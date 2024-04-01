#include "MainRenderPass.h"

void MainRenderPass::setup()
{
	lightManager = &LightManager::getInstance();
	renderContext = &RenderContext::getInstance();
	resourceManager = &ResourceManager::getInstance();

	frameBuffer = renderContext->frameBuffer;
	ssaoBlurTexture = renderContext->ssaoBlurTexture;
	depthTexture = renderContext->depthTexture;

	camera = renderContext->camera;
	clusterGrid = renderContext->clusterGrid;

	defaultShaderProgram = &resourceManager->getShaderProgram("default");

	setInitialUniforms();
}

void MainRenderPass::execute()
{
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	defaultShaderProgram->use();
	defaultShaderProgram->setUniform("viewMatrix", camera->getViewMatrix());
	defaultShaderProgram->setUniform("viewPos", camera->position);
	defaultShaderProgram->resetLights();
	lightManager->setLightUniforms(*defaultShaderProgram);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);
	defaultShaderProgram->setUniform("ssaoBlurTexture", 5);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	defaultShaderProgram->setUniform("depthTexture", 6);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderContext->irradianceCubemap);
	defaultShaderProgram->setUniform("irradianceCubemap", 7);
	glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_CUBE_MAP, renderContext->prefilterCubemap);
	defaultShaderProgram->setUniform("prefilteredEnvCubemap", 8);
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_2D, renderContext->brdfLUTTexture);
	defaultShaderProgram->setUniform("brdfLUT", 9);

	glEnable(GL_CULL_FACE);
	renderContext->renderAll(*defaultShaderProgram);
	glDisable(GL_CULL_FACE);
}

void MainRenderPass::setInitialUniforms() const
{
	const glm::mat4 projectionMatrix = renderContext->projectionMatrix;
	defaultShaderProgram->use();
	defaultShaderProgram->setUniform("projectionMatrix", projectionMatrix);
	clusterGrid->setUniforms(*defaultShaderProgram);
}
