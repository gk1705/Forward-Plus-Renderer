#pragma once

#include "IRenderPass.h"
#include "ResourceManager.h"

class ShadowPass : public IRenderPass
{
public:
	ShaderProgram* depthMapShaderProgram = nullptr;
	ShaderProgram* depthCubeMapShaderProgram = nullptr;

	void setup() override;

	void execute() override;
};
