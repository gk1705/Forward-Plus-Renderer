#pragma once

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"
#include "ShaderProgram.h"

class Model
{
public:
	std::vector<Mesh> meshes;

	explicit Model(const char* path);

	void draw(ShaderProgram& shaderProgram) const;

	void cleanup() const;

private:
	std::vector<Texture> loadedTextures;
	std::string directory;

	void loadModel(const std::string& path);
	void processNode(const aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName);
	static unsigned int loadTextureFromFile(const char* path, const std::string& directory, aiTextureType type);
};
