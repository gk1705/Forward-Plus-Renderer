#include "PostProcessingPass.h"

void PostProcessingPass::setup()
{
	renderContext = &RenderContext::getInstance();
	resourceManager = &ResourceManager::getInstance();

	pingPongTextures[0] = renderContext->pingPongTextures[0];
	pingPongTextures[1] = renderContext->pingPongTextures[1];
	screenTexture = renderContext->screenTexture;

	frameBuffer = renderContext->frameBuffer;
	intermediateFBO = renderContext->intermediateFBO;

	screenSpaceQuad = renderContext->screenSpaceQuad;

	screenQuadShaderProgram = &resourceManager->getShaderProgram("screenQuad"); // TODO: change name to post processing
}

void PostProcessingPass::execute()
{
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	screenQuadShaderProgram->use();
	screenQuadShaderProgram->setUniform("gamma", 2.2f);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, pingPongTextures[!renderContext->horizontal]);
	screenQuadShaderProgram->setUniform("bloomTexture", 1);
	screenSpaceQuad->texture = renderContext->screenTexture;
	screenSpaceQuad->draw();
}
