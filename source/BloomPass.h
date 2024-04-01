#pragma once

#include "Application.h"
#include "IRenderPass.h"
#include "RenderContext.h"
#include "ResourceManager.h"
#include "ScreenSpaceQuad.h"

class BloomPass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	unsigned int frameBuffer;
	unsigned int bloomFBO;

	unsigned int pingPongFBOs[2];
	unsigned int pingPongTextures[2];

	unsigned int bloomTexture;

	ShaderProgram* bloomShaderProgram = nullptr;

	ScreenSpaceQuad* screenSpaceQuad = nullptr;

	RenderContext* renderContext = nullptr;
	ResourceManager* resourceManager = nullptr;
};
