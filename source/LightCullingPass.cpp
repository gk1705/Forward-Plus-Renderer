#include "LightCullingPass.h"

void LightCullingPass::setup()
{
	resourceManager = &ResourceManager::getInstance();
	renderContext = &RenderContext::getInstance();

	clusterGrid = RenderContext::getInstance().clusterGrid;
	camera = RenderContext::getInstance().camera;

	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;
	depthTexture = renderContext->depthTexture;

	resetClustersShaderProgram = &resourceManager->getShaderProgram("resetClusters");
	activeClusterSelectionShaderProgram = &resourceManager->getShaderProgram("activeClusterSelection");
	compactClustersShaderProgram = &resourceManager->getShaderProgram("compactClusters");
	clusteredLightCullingShaderProgram = &resourceManager->getShaderProgram("clusteredLightCulling");

	setInitialUniforms();
}

void LightCullingPass::execute()
{
	const int width = renderContext->WIDTH;
	const int height = renderContext->HEIGHT;

	// reset before anything else
	resetClustersShaderProgram->use();
	glDispatchCompute(16, 9, 24);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterGrid.activeClustersSSBO);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, clusterGrid.activeClusters.size() * sizeof(uint32_t), clusterGrid.activeClusters.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterGrid.compactClustersSSBO);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, clusterGrid.compactClusters.size() * sizeof(uint32_t), clusterGrid.compactClusters.data());*/

	// run the compute shader to generate the active clusters
	activeClusterSelectionShaderProgram->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	activeClusterSelectionShaderProgram->setUniform("depthTexture", 0);
	glDispatchCompute(width, height, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterGrid.activeClustersSSBO);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, clusterGrid.activeClusters.size() * sizeof(uint32_t), clusterGrid.activeClusters.data());*/
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, clusterGrid.activeClusters.size() * sizeof(uint32_t), clusterGrid.activeClusters.data());

	compactClustersShaderProgram->use();
	glDispatchCompute(16, 9, 24);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	/*glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterGrid.compactClustersSSBO);
		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, clusterGrid.compactClusters.size() * sizeof(uint32_t), clusterGrid.compactClusters.data());*/
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterGrid->activeClusterCountSSBO);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32_t), &clusterGrid->activeClusterCount);

	// clustered light culling goes here
	clusteredLightCullingShaderProgram->use();
	const glm::mat4 viewMatrix = camera->getViewMatrix();
	clusteredLightCullingShaderProgram->setUniform("viewMatrix", viewMatrix);
	glDispatchCompute(clusterGrid->activeClusterCount, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LightCullingPass::setInitialUniforms() const
{
	clusterGrid->setUniforms(*activeClusterSelectionShaderProgram);
	clusterGrid->setUniforms(*compactClustersShaderProgram);
	clusterGrid->setUniforms(*resetClustersShaderProgram);
}
