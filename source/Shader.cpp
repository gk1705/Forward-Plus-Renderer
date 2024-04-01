#include "Shader.h"

#include <fstream>
#include <iostream>

Shader::Shader(const char* filePath, GLenum type): type(type)
{
	const std::vector<char> shaderCode = readFile(filePath);
	const char* shaderCodePtr = shaderCode.data();
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shaderCodePtr, NULL);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
}

std::vector<char> Shader::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}
	const size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}
