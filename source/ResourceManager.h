#pragma once

#include <unordered_map>

#include "ShaderProgram.h"
#include "RenderContext.h"

class ResourceManager : public Singleton<ResourceManager>
{
	friend class Singleton<ResourceManager>;
public:
	ShaderProgram& getShaderProgram(const std::string& key);

	void init();
	void cleanup() const;

protected:
	ResourceManager() = default;

private:
	std::unordered_map<std::string, ShaderProgram> shaderPrograms;
};
