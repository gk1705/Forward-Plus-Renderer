#include "RenderingPipelineManager.h"

#include <cassert>

void RenderingPipelineManager::addRenderingPass(IRenderPass* renderPass)
{
	if (renderPasses.find(renderPass->type) != renderPasses.end())
	{
		assert(false);
	}

	renderPasses.insert(std::pair<RenderPassType, IRenderPass*>(renderPass->type, renderPass));
}

void RenderingPipelineManager::removeComponent(RenderPassType type)
{
	if (renderPasses.find(type) != renderPasses.end())
	{
		renderPasses.erase(type);
	}
}

void RenderingPipelineManager::executePipeline()
{
	for (auto it = renderPasses.begin(); it != renderPasses.end(); ++it)
	{
		it->second->execute();
	}
}
