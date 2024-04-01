#include "BloomPass.h"

void BloomPass::setup()
{
	renderContext = &RenderContext::getInstance();
	resourceManager = &ResourceManager::getInstance();

	frameBuffer = renderContext->frameBuffer;
	bloomFBO = renderContext->bloomFBO;

	pingPongFBOs[0] = renderContext->pingPongFBOs[0];
	pingPongFBOs[1] = renderContext->pingPongFBOs[1];
	pingPongTextures[0] = renderContext->pingPongTextures[0];
	pingPongTextures[1] = renderContext->pingPongTextures[1];

	bloomTexture = renderContext->bloomTexture;

	bloomShaderProgram = &resourceManager->getShaderProgram("bloom");

	screenSpaceQuad = renderContext->screenSpaceQuad;
}

void BloomPass::execute()
{
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;

	glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
	glReadBuffer(GL_COLOR_ATTACHMENT1);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, bloomFBO);
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	bool horizontal = true;
	bool firstIteration = true;
	constexpr int amount = 10;
	bloomShaderProgram->use();
	for (int i = 0; i < amount; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, pingPongFBOs[horizontal]);
		bloomShaderProgram->setUniform("horizontal", horizontal);
		const unsigned int texture = firstIteration ? bloomTexture : pingPongTextures[!horizontal];
		screenSpaceQuad->texture = texture;
		screenSpaceQuad->draw();
		horizontal = !horizontal;
		if (firstIteration)
		{
			firstIteration = false;
		}
	}
	renderContext->horizontal = horizontal;
}
