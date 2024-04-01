#include "ClusterGrid.h"

ClusterGrid::ClusterGrid(float zNear, float zFar, glm::uvec4 tileSizes, glm::vec2 screenDimensions): zNear(zNear), zFar(zFar), tileSizes(tileSizes), screenDimensions(screenDimensions)
{
	clusters.resize(tileSizes.x * tileSizes.y * tileSizes.z);

	activeClusters.resize(clusters.size(), 0);
	compactClusters.resize(clusters.size(), 0);

	lightGrids.resize(clusters.size(), { 0, 0 });
	lightIndices.resize(clusters.size(), 0); // could be that we need to expand this
}

void ClusterGrid::init(const glm::mat4& inverseProjectionMatrix_)
{
	lightManager = &LightManager::getInstance();

	inverseProjectionMatrix = inverseProjectionMatrix_;
	createClusters();

	glGenBuffers(1, &activeClustersSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, activeClustersSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, activeClusters.size() * sizeof(uint32_t), activeClusters.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, activeClustersSSBO);

	glGenBuffers(1, &compactClustersSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, compactClustersSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, compactClusters.size() * sizeof(uint32_t), compactClusters.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, compactClustersSSBO);

	glGenBuffers(1, &activeClusterCountSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, activeClusterCountSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &activeClusterCount, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, activeClusterCountSSBO);

	glGenBuffers(1, &clusterAABBSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterAABBSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, clusters.size() * sizeof(Cluster), clusters.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, clusterAABBSSBO);

	glGenBuffers(1, &lightGridsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightGridsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightGrids.size() * sizeof(LightGrid), lightGrids.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, lightGridsSSBO);

	lights.resize(lightManager->pointLights.size());
	for (size_t i = 0; i < lightManager->pointLights.size(); i++)
	{
		lights[i].position = lightManager->pointLights[i].position;
		lights[i].radius = lightManager->pointLights[i].getRadius();
	}

	glGenBuffers(1, &lightsSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightsSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lights.size() * sizeof(Light), lights.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, lightsSSBO);

	glGenBuffers(1, &lightIndicesSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndicesSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightIndices.size() * sizeof(uint32_t), lightIndices.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, lightIndicesSSBO);

	glGenBuffers(1, &globalLightIndexCountSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, globalLightIndexCountSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(unsigned int), &globalLightIndexCount, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, globalLightIndexCountSSBO);
}

void ClusterGrid::cleanup() const
{
	glDeleteBuffers(1, &activeClustersSSBO);
	glDeleteBuffers(1, &compactClustersSSBO);
	glDeleteBuffers(1, &activeClusterCountSSBO);
	glDeleteBuffers(1, &clusterAABBSSBO);
	glDeleteBuffers(1, &lightGridsSSBO);
	glDeleteBuffers(1, &lightsSSBO);
	glDeleteBuffers(1, &lightIndicesSSBO);
	glDeleteBuffers(1, &globalLightIndexCountSSBO);
}

void ClusterGrid::setUniforms(ShaderProgram& shader) const
{
	shader.use();
	shader.setUniform("numClustersX", (int)tileSizes.x);
	shader.setUniform("numClustersY", (int)tileSizes.y);
	shader.setUniform("numClustersZ", (int)tileSizes.z);
	shader.setUniform("clusterSizePx", (int)tileSizes.w);
	shader.setUniform("screenDimensions", screenDimensions);
	shader.setUniform("zNear", zNear);
	shader.setUniform("zFar", zFar);
}

void ClusterGrid::createClusters()
{
	for (int z = 0; z < tileSizes.z; z++)
	{
		for (int y = 0; y < tileSizes.y; y++)
		{
			for (int x = 0; x < tileSizes.x; x++)
			{
				const glm::vec3 gridIndex = glm::vec3(x, y, z);
				createCluster(gridIndex);
			}
		}
	}
}

void ClusterGrid::createCluster(glm::vec3 gridIndex)
{
	// the grid positions are going to be calculated in view space
	constexpr glm::vec3 eyePos = glm::vec3(0);

	int tileSizePixel = static_cast<int>(tileSizes.w);
	int tileIndex = gridIndex.x + gridIndex.y * tileSizes.x + gridIndex.z * tileSizes.x * tileSizes.y;

	// bottom left corner
	const glm::vec4 minPoint = glm::vec4(gridIndex.x * tileSizePixel, gridIndex.y * tileSizePixel, -1.0f, 1.0f);
	// top right corner
	const glm::vec4 maxPoint = glm::vec4((gridIndex.x + 1) * tileSizePixel, (gridIndex.y + 1) * tileSizePixel, 1.0f, 1.0f);

	glm::vec3 minPointView = glm::vec3(screenToView(minPoint));
	glm::vec3 maxPointView = glm::vec3(screenToView(maxPoint));

	float minZ = -zNear * pow(zFar / zNear, gridIndex.z / tileSizes.z);
	float maxZ = -zNear * pow(zFar / zNear, (gridIndex.z + 1) / tileSizes.z);

	glm::vec3 minPointNear = lineIntersectionToZPlane(eyePos, minPointView, minZ);
	glm::vec3 maxPointNear = lineIntersectionToZPlane(eyePos, maxPointView, minZ);
	glm::vec3 minPointFar = lineIntersectionToZPlane(eyePos, minPointView, maxZ);
	glm::vec3 maxPointFar = lineIntersectionToZPlane(eyePos, maxPointView, maxZ);

	const glm::vec3 minPointAABB = minimumVector(minimumVector(minPointNear, minPointFar), minimumVector(maxPointNear, maxPointFar));
	const glm::vec3 maxPointAABB = maximumVector(maximumVector(minPointNear, minPointFar), maximumVector(maxPointNear, maxPointFar));

	clusters[tileIndex].minPoint = glm::vec4(minPointAABB, 1.0f);
	clusters[tileIndex].maxPoint = glm::vec4(maxPointAABB, 1.0f);
}

glm::vec4 ClusterGrid::screenToView(glm::vec4 screenPoint) const
{
	glm::vec2 ndc = glm::vec2(screenPoint) / screenDimensions;
	ndc *= 2.0 - 1.0;
	const glm::vec4 clip = glm::vec4(ndc.x, ndc.y, screenPoint.z, screenPoint.w);

	glm::vec4 view = inverseProjectionMatrix * clip;
	view /= view.w;
	return view;
}

glm::vec3 ClusterGrid::lineIntersectionToZPlane(glm::vec3 lineStart, glm::vec3 lineEnd, float z)
{
	const float t = (z - lineStart.z) / (lineEnd.z - lineStart.z);
	return lineStart + t * (lineEnd - lineStart);
}

glm::vec3 ClusterGrid::minimumVector(const glm::vec3& a, const glm::vec3& b)
{
	return { std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z) };
}

glm::vec3 ClusterGrid::maximumVector(const glm::vec3& a, const glm::vec3& b)
{
	return { std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z) };
}

