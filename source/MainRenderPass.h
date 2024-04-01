#pragma once

#include "IRenderPass.h"
#include "LightManager.h"
#include "ResourceManager.h"
#include "ClusterGrid.h"
#include "Camera.h"

class MainRenderPass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	unsigned int frameBuffer;
	unsigned int ssaoBlurTexture;
	unsigned int depthTexture;

	ShaderProgram* defaultShaderProgram = nullptr;
	Camera* camera = nullptr;
	ClusterGrid* clusterGrid = nullptr;

	LightManager* lightManager = nullptr;
	RenderContext* renderContext = nullptr;
	ResourceManager* resourceManager = nullptr;

	void setInitialUniforms() const;
};
