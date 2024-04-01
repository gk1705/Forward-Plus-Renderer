#pragma once
#include <unordered_map>

#include "IRenderPass.h"

class RenderingPassFactory
{
public:
	template <typename T, typename... Args>
	static T* createRenderingPass(RenderPassType type, Args&&... args)
	{
		if (renderPassMap.find(type) != renderPassMap.end())
		{
			return static_cast<T*>(renderPassMap[type]);
		}

		T* component = new T(std::forward<Args>(args)...);
		component->setup();
		component->type = type;
		renderPassMap.insert(std::pair<RenderPassType, T*>(type, component));
		return component;
	}

	template <typename T>
	static T* getRenderingPass(RenderPassType type)
	{
		if (renderPassMap.find(type) != renderPassMap.end())
		{
			return renderPassMap[type];
		}

		return nullptr; // needs a log here
	}

	static void removeRenderingPass(RenderPassType type)
	{
		if (renderPassMap.find(type) != renderPassMap.end())
		{
			delete renderPassMap[type];
			renderPassMap[type] = nullptr;
			renderPassMap.erase(type);
		}
	}

	static void clearRenderingPasses()
	{
		for (auto it = renderPassMap.begin(); it != renderPassMap.end(); ++it)
		{
			delete it->second;
			it->second = nullptr;
		}

		renderPassMap.clear();
	}

private:
	static std::unordered_map<RenderPassType, IRenderPass*> renderPassMap;
};
