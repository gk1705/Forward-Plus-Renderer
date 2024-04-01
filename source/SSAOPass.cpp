#include "SSAOPass.h"

void SSAOPass::setup()
{
	renderContext = &RenderContext::getInstance();
	resourceManager = &ResourceManager::getInstance();

	ssaoFBO = renderContext->ssaoFBO;
	ssaoTexture = renderContext->ssaoTexture;
	ssaoBlurFBO = renderContext->ssaoBlurFBO;
	ssaoBlurTexture = renderContext->ssaoBlurTexture;
	depthTexture = renderContext->depthTexture;

	ssaoShaderProgram = &resourceManager->getShaderProgram("ssao");
	ssaoBlurShaderProgram = &resourceManager->getShaderProgram("ssaoBlur");

	screenSpaceQuad = renderContext->screenSpaceQuad;

	setupSSAO();
	renderContext->noiseTexture = noiseTexture;

	setInitialUniforms();
}

void SSAOPass::execute()
{
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	ssaoShaderProgram->use();
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	ssaoShaderProgram->setUniform("noiseTexture", 1);
	screenSpaceQuad->texture = depthTexture;
	screenSpaceQuad->draw();

	// blur the ssao texture
	ssaoBlurShaderProgram->use();
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	screenSpaceQuad->texture = ssaoTexture;
	screenSpaceQuad->draw();
}

void SSAOPass::setupSSAO()
{
	std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
	std::default_random_engine generator;
	for (int i = 0; i < 64; i++)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		float scale = static_cast<float>(i) / 64.0f;
		scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	std::vector<glm::vec3> ssaoNoise;
	for (int i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0f - 1.0f,
			randomFloats(generator) * 2.0f - 1.0f,
			0.0f);
		ssaoNoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void SSAOPass::setInitialUniforms() const
{
	const glm::mat4 projectionMatrix = renderContext->projectionMatrix;
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;

	ssaoShaderProgram->use();
	ssaoShaderProgram->setUniform("projectionMatrix", projectionMatrix);
	ssaoShaderProgram->setUniform("inverseProjectionMatrix", glm::inverse(projectionMatrix));
	for (int i = 0; i < ssaoKernel.size(); i++)
	{
		ssaoShaderProgram->setUniform("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
	}
	ssaoShaderProgram->setUniform("kernelSize", 64);
	ssaoShaderProgram->setUniform("kernelRadius", 0.5f);
	ssaoShaderProgram->setUniform("bias", 0.025f);
	ssaoShaderProgram->setUniform("noiseScale", glm::vec2(width / 4.0f, height / 4.0f));
}
