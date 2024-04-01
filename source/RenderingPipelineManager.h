#pragma once

#include <map>

#include "IRenderPass.h"

class RenderingPipelineManager
{
public:
	void addRenderingPass(IRenderPass* renderPass);
	void removeComponent(RenderPassType type);
	void executePipeline();

private:
	std::map<RenderPassType, IRenderPass*> renderPasses; // ordered by enum type
};