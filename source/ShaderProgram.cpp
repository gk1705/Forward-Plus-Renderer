#include "ShaderProgram.h"

ShaderProgram::ShaderProgram(const std::initializer_list<Shader> shaders)
{
	program = glCreateProgram();
	for (const Shader& shader : shaders)
	{
		glAttachShader(program, shader.shader);
	}
	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	for (const Shader& shader : shaders)
	{
		glDeleteShader(shader.shader);
	}
}

void ShaderProgram::cleanup() const
{
	glDeleteProgram(program);
}

unsigned ShaderProgram::getUniformLocation(const std::string& name)
{
	if (cachedUniformLocations.find(name) == cachedUniformLocations.end())
	{
		cachedUniformLocations[name] = glGetUniformLocation(program, name.c_str());
	}
	return cachedUniformLocations[name];
}

void ShaderProgram::setUniform(const std::string& name, int value)
{
	glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, float value)
{
	glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z)
{
	glUniform3f(getUniformLocation(name), x, y, z);
}

void ShaderProgram::setUniform(const std::string& name, float x, float y, float z, float w)
{
	glUniform4f(getUniformLocation(name), x, y, z, w);
}

void ShaderProgram::setUniform(const std::string& name, const glm::mat4& value)
{
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec2& value)
{
	glUniform2fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3& value)
{
	glUniform3fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec4& value)
{
	glUniform4fv(getUniformLocation(name), 1, &value[0]);
}

void ShaderProgram::setUniform(const std::string& name, const glm::vec3* values, int count)
{
	glUniform3fv(getUniformLocation(name), count, &values[0][0]);
}

void ShaderProgram::use() const
{
	glUseProgram(program);
}

void ShaderProgram::setLight(const DirectionalLight& light)
{
	const std::string index = std::to_string(directionalLightCount++);
	setUniform("directionalLights[" + index + "].direction", light.direction);
	setUniform("directionalLights[" + index + "].color", light.color);
	setUniform("directionalLights[" + index + "].ambientIntensity", light.ambientIntensity);
	setUniform("directionalLights[" + index + "].diffuseIntensity", light.diffuseIntensity);
	setUniform("directionalLights[" + index + "].specularIntensity", light.specularIntensity);
	setUniform("directionalLightCount", directionalLightCount);
}

void ShaderProgram::setLight(const PointLight& light)
{
	const std::string index = std::to_string(pointLightCount++);
	setUniform("pointLights[" + index + "].position", light.position);
	setUniform("pointLights[" + index + "].color", light.color);
	setUniform("pointLights[" + index + "].ambientIntensity", light.ambientIntensity);
	setUniform("pointLights[" + index + "].diffuseIntensity", light.diffuseIntensity);
	setUniform("pointLights[" + index + "].specularIntensity", light.specularIntensity);
	setUniform("pointLights[" + index + "].constant", light.constant);
	setUniform("pointLights[" + index + "].linear", light.linear);
	setUniform("pointLights[" + index + "].quadratic", light.quadratic);
	setUniform("pointLightCount", pointLightCount);
}

void ShaderProgram::setLight(const SpotLight& light)
{
	const std::string index = std::to_string(spotLightCount++);
	setUniform("spotLights[" + index + "].position", light.position);
	setUniform("spotLights[" + index + "].direction", light.direction);
	setUniform("spotLights[" + index + "].color", light.color);
	setUniform("spotLights[" + index + "].ambientIntensity", light.ambientIntensity);
	setUniform("spotLights[" + index + "].diffuseIntensity", light.diffuseIntensity);
	setUniform("spotLights[" + index + "].specularIntensity", light.specularIntensity);
	setUniform("spotLights[" + index + "].constant", light.constant);
	setUniform("spotLights[" + index + "].linear", light.linear);
	setUniform("spotLights[" + index + "].quadratic", light.quadratic);
	setUniform("spotLights[" + index + "].cutOff", light.cutOff);
	setUniform("spotLights[" + index + "].outerCutOff", light.outerCutOff);
	setUniform("spotLightCount", spotLightCount);
}

void ShaderProgram::resetLights()
{
	setUniform("directionalLightCount", 0);
	setUniform("pointLightCount", 0);
	setUniform("spotLightCount", 0);

	directionalLightCount = 0;
	pointLightCount = 0;
	spotLightCount = 0;
}
