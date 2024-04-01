#pragma once

#include "IRenderPass.h"
#include "ResourceManager.h"
#include "ShaderProgram.h"

class DepthPrePass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	unsigned int depthFBOMultisample;
	unsigned int depthTextureMultisampled;
	unsigned int depthFBO;
	unsigned int depthTexture;
	unsigned int frameBuffer;

	ShaderProgram* depthPassShaderProgram = nullptr;
	RenderContext* renderContext = nullptr;
	ResourceManager* resourceManager = nullptr;

	void setUniforms() const;
	void setInitialUniforms() const;
};
