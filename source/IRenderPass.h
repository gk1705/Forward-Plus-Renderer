#pragma once

enum class RenderPassType
{
	DepthPrePass,
	ShadowPass,
	SSAOPass,
	LightCullingPass,
	MainRenderPass,
	EnvironmentMapPass,
	BloomPass,
	PostProcessingPass,
};

class IRenderPass
{
public:
	virtual ~IRenderPass() = default;
	virtual void setup() = 0;
	virtual void execute() = 0;

	RenderPassType type;
};