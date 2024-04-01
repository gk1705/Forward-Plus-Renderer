#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "LightManager.h"

struct Cluster
{
	// this is essentially an AABB
	glm::vec4 minPoint;
	glm::vec4 maxPoint;
};

struct LightGrid
{
	unsigned int offset;
	unsigned int count;
};

struct Light
{
	glm::vec3 position;
	float radius;
};

class ClusterGrid
{
public:
	unsigned int activeClusterCountSSBO;
	unsigned int activeClusterCount;

	ClusterGrid() = default;
	ClusterGrid(float zNear, float zFar, glm::uvec4 tileSizes, glm::vec2 screenDimensions);

	void init(const glm::mat4& inverseProjectionMatrix_);
	void cleanup() const;

	void setUniforms(ShaderProgram& shader) const;

private:
	float zNear;
	float zFar;

	unsigned int globalLightIndexCount;

	glm::uvec4 tileSizes;
	glm::mat4 inverseProjectionMatrix;
	glm::vec2 screenDimensions;

	std::vector<Cluster> clusters;
	std::vector<uint32_t> activeClusters; // switch to packed bools in the future
	std::vector<uint32_t> compactClusters;
	std::vector<LightGrid> lightGrids;
	std::vector<Light> lights;
	std::vector<uint32_t> lightIndices;

	unsigned int activeClustersSSBO;
	unsigned int compactClustersSSBO;
	unsigned int clusterAABBSSBO;
	unsigned int lightGridsSSBO;
	unsigned int lightsSSBO;
	unsigned int lightIndicesSSBO;
	unsigned int globalLightIndexCountSSBO;

	LightManager* lightManager = nullptr;

	void createClusters();
	void createCluster(glm::vec3 gridIndex);

	glm::vec4 screenToView(glm::vec4 screenPoint) const;

	static glm::vec3 lineIntersectionToZPlane(glm::vec3 lineStart, glm::vec3 lineEnd, float z);
	static glm::vec3 minimumVector(const glm::vec3& a, const glm::vec3& b);
	static glm::vec3 maximumVector(const glm::vec3& a, const glm::vec3& b);
};