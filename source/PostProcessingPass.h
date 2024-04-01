#pragma once

#include "IRenderPass.h"
#include "RenderContext.h"
#include "ResourceManager.h"

class PostProcessingPass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	unsigned int pingPongTextures[2];
	unsigned int screenTexture;

	unsigned int frameBuffer;
	unsigned int intermediateFBO;

	ShaderProgram* screenQuadShaderProgram = nullptr;

	ScreenSpaceQuad* screenSpaceQuad = nullptr;

	RenderContext* renderContext = nullptr;
	ResourceManager* resourceManager = nullptr;
};
