#include "RenderContext.h"

#include <glm/ext/matrix_clip_space.hpp>

void RenderContext::init()
{
	projectionMatrix = glm::perspective(glm::radians(45.0f), static_cast<float>(WIDTH) / static_cast<float>(HEIGHT), 0.1f, 100.0f);

	loadModels();
	createActors();
	setupFrameBuffersAndTextures();
}

void RenderContext::loadModels()
{
	const Model backpackModel("../assets/models/backpack/backpack.obj");
	const Model floorModel("../assets/models/floor/floor.obj");
	models.push_back(backpackModel);
	models.push_back(floorModel);
}

void RenderContext::createActors()
{
	Model& backpackModel = models[0];
	Model& floorModel = models[1];

	constexpr glm::mat4 modelMatrix1 = glm::mat4(1.0f);
	const Actor actor1(backpackModel, modelMatrix1);
	const glm::mat4 modelMatrix2 = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, -1.0f, -2.0f));
	Actor actor2(backpackModel, modelMatrix2);
	actor2.isOutlined = false;
	const glm::mat4 modelMatrix3 = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, -2.0f));
	Actor actor3(backpackModel, modelMatrix3);
	actor3.isOutlined = false;

	actors.push_back(actor1);
	actors.push_back(actor2);
	actors.push_back(actor3);

	glm::mat4 modelMatrix4 = glm::mat4(1.0f);
	modelMatrix4 = glm::scale(modelMatrix4, glm::vec3(50.f, 1.f, 50.0f));
	modelMatrix4 = glm::translate(modelMatrix4, glm::vec3(0, -2, 0));
	const Actor actor4(floorModel, modelMatrix4);
	actors.push_back(actor4);

	glm::mat4 modelMatrix5 = glm::mat4(1.0f);
	modelMatrix5 = glm::translate(modelMatrix5, glm::vec3(0, 3.5f, -1.5f));
	modelMatrix5 = glm::scale(modelMatrix5, glm::vec3(0.2f));
	const Actor actor5(backpackModel, modelMatrix5);
	actors.push_back(actor5);
}

void RenderContext::setupFrameBuffersAndTextures()
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// attach multisampled color buffer
	glGenTextures(1, &frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WIDTH, HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, frameBufferTexture, 0);

	// attach another multisampled color buffer for bloom that will only render the bright parts of the scene
	glGenTextures(1, &bloomTextureMultisampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, bloomTextureMultisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, WIDTH, HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, bloomTextureMultisampled, 0);

	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH32F_STENCIL8, WIDTH, HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);

	constexpr unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	glGenFramebuffers(1, &depthFBOMultisample);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBOMultisample);

	glGenTextures(1, &depthTextureMultisampled);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depthTextureMultisampled);
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT32F, WIDTH, HEIGHT, GL_TRUE);
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depthTextureMultisampled, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to initialize framebuffer" << std::endl;
		assert(false);
	}

	glGenFramebuffers(1, &intermediateFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);

	glGenTextures(1, &screenTexture);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);

	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &bloomFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO);

	glGenTextures(1, &bloomTexture);
	glBindTexture(GL_TEXTURE_2D, bloomTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssaoTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	glGenTextures(1, &ssaoBlurTexture);
	glBindTexture(GL_TEXTURE_2D, ssaoBlurTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Failed to initialize intermediate framebuffer" << std::endl;
		// cleanup ?
		assert(false);
	}

	glGenFramebuffers(2, pingPongFBOs);
	glGenTextures(2, pingPongTextures);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBOs[i]);
		glBindTexture(GL_TEXTURE_2D, pingPongTextures[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingPongTextures[i], 0
		);
	}
}

void RenderContext::cleanup() const
{
	for (auto& model : models)
	{
		model.cleanup();
	}

	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &frameBufferTexture);
	glDeleteRenderbuffers(1, &renderBuffer);

	glDeleteFramebuffers(1, &depthFBOMultisample);
	glDeleteTextures(1, &depthTextureMultisampled);
	glDeleteFramebuffers(1, &depthFBO);
	glDeleteTextures(1, &depthTexture);

	glDeleteFramebuffers(1, &intermediateFBO);
	glDeleteTextures(1, &screenTexture);

	glDeleteFramebuffers(1, &bloomFBO);
	glDeleteTextures(1, &bloomTextureMultisampled);
	glDeleteTextures(1, &bloomTexture);

	glDeleteFramebuffers(1, &ssaoFBO);
	glDeleteTextures(1, &ssaoTexture);

	glDeleteFramebuffers(1, &ssaoBlurFBO);
	glDeleteTextures(1, &ssaoBlurTexture);

	glDeleteFramebuffers(2, pingPongFBOs);
	glDeleteTextures(2, pingPongTextures);
}

void RenderContext::renderNonOutlinedActors(ShaderProgram& shaderProgram) const
{
	for (auto& actor : actors)
	{
		if (!actor.isOutlined)
		{
			actor.draw(shaderProgram);
		}
	}
}

void RenderContext::renderOutlinedActors(ShaderProgram& shaderProgram) const
{
	for (auto& actor : actors)
	{
		if (actor.isOutlined)
		{
			actor.draw(shaderProgram);
		}
	}
}

void RenderContext::renderAll(ShaderProgram& shaderProgram) const
{
	for (auto& actor : actors)
	{
		actor.draw(shaderProgram);
	}
}

