#pragma once

#include "Camera.h"
#include "ClusterGrid.h"
#include "IRenderPass.h"
#include "RenderContext.h"
#include "ResourceManager.h"
#include "ShaderProgram.h"

class LightCullingPass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	unsigned int depthTexture;

	ShaderProgram* resetClustersShaderProgram = nullptr;
	ShaderProgram* activeClusterSelectionShaderProgram = nullptr;
	ShaderProgram* compactClustersShaderProgram = nullptr;
	ShaderProgram* clusteredLightCullingShaderProgram = nullptr;

	ClusterGrid* clusterGrid = nullptr;
	Camera* camera = nullptr;

	RenderContext* renderContext = &RenderContext::getInstance();
	ResourceManager* resourceManager = nullptr;

	void setInitialUniforms() const;
};
