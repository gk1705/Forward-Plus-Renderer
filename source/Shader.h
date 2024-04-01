#pragma once
#include <ostream>
#include <vector>
#include <glad/glad.h>

class Shader
{
public:
	GLenum type;
	unsigned int shader;

	Shader(const char* filePath, GLenum type);

	static std::vector<char> readFile(const std::string& filename);
};
