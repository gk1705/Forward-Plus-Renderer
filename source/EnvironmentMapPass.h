#pragma once

#include "Camera.h"
#include "EnvironmentMap.h"
#include "IRenderPass.h"
#include "RenderContext.h"
#include "ResourceManager.h"

class EnvironmentMapPass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	EnvironmentMap environmentMap;

	Camera* camera = nullptr;
	ShaderProgram* environmentMapShaderProgram = nullptr;
	ShaderProgram* skyboxShaderProgram = nullptr;
	ShaderProgram* cubeMapConvolutionShaderProgram = nullptr;
	ShaderProgram* cubeMapPrefilterShaderProgram = nullptr;
	ShaderProgram* brdfLutShaderProgram = nullptr;

	RenderContext* renderContext = nullptr;
	ResourceManager* resourceManager = nullptr;

	void setInitialUniforms() const;
};
