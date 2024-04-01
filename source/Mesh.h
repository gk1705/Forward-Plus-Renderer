#pragma once

#include <vector>

#include "ShaderProgram.h"
#include "Vertex.h"
#include "Texture.h"

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	void cleanup() const;

	std::vector<float> getVertices() const;

	void draw(ShaderProgram& shaderProgram) const;

private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	void setupMesh();
};