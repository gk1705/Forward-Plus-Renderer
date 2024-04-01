#pragma once
#include <initializer_list>
#include <string>
#include <unordered_map>

#include "Shader.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class ShaderProgram
{
public:
	unsigned int program = 0;

	ShaderProgram(const std::initializer_list<Shader> shaders);
	ShaderProgram() = default;

	void cleanup() const;

	void setUniform(const std::string& name, int value);
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, float x, float y, float z);
	void setUniform(const std::string& name, float x, float y, float z, float w);
	void setUniform(const std::string& name, const glm::mat4& value);
	void setUniform(const std::string& name, const glm::vec2& value);
	void setUniform(const std::string& name, const glm::vec3& value);
	void setUniform(const std::string& name, const glm::vec4& value);
	void setUniform(const std::string& name, const glm::vec3* values, int count);
	void use() const;

	void setLight(const DirectionalLight& light);
	void setLight(const PointLight& light);
	void setLight(const SpotLight& light);
	void resetLights();

private:
	unsigned int getUniformLocation(const std::string& name);

	int directionalLightCount = 0;
	int pointLightCount = 0;
	int spotLightCount = 0;

	std::unordered_map<std::string, unsigned int> cachedUniformLocations;
};