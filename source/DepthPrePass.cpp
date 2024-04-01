#include "DepthPrePass.h"

#include "Camera.h"

void DepthPrePass::setup()
{
	renderContext = &RenderContext::getInstance();
	resourceManager = &ResourceManager::getInstance();

	depthFBOMultisample = renderContext->depthFBOMultisample;
	depthTextureMultisampled = renderContext->depthTextureMultisampled;
	depthFBO = renderContext->depthFBO;
	depthTexture = renderContext->depthTexture;
	frameBuffer = renderContext->frameBuffer;

	depthPassShaderProgram = &resourceManager->getShaderProgram("depthPass");
	setInitialUniforms();
}

void DepthPrePass::execute()
{
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;

	glDepthFunc(GL_LESS);
	glBindFramebuffer(GL_FRAMEBUFFER, depthFBOMultisample);
	glViewport(0, 0, width, height);
	glClearColor(0.f, 1.f, 0.f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);

	setUniforms();

	glEnable(GL_CULL_FACE);
	renderContext->renderNonOutlinedActors(*depthPassShaderProgram);
	glDisable(GL_CULL_FACE);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, depthFBOMultisample);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, depthFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void DepthPrePass::setUniforms() const
{
	if (depthPassShaderProgram == nullptr) assert(false);

	const glm::mat4 viewMatrix = renderContext->camera->getViewMatrix();

	depthPassShaderProgram->use();
	depthPassShaderProgram->setUniform("viewMatrix", viewMatrix);
}

void DepthPrePass::setInitialUniforms() const
{
	depthPassShaderProgram->use();
	depthPassShaderProgram->setUniform("projectionMatrix", renderContext->projectionMatrix);
}
