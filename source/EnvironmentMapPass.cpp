#include "EnvironmentMapPass.h"

void EnvironmentMapPass::setup()
{
	environmentMap.init();

	renderContext = &RenderContext::getInstance();
	resourceManager = &ResourceManager::getInstance();

	camera = renderContext->camera;
	environmentMapShaderProgram = &resourceManager->getShaderProgram("environmentMap");
	skyboxShaderProgram = &resourceManager->getShaderProgram("skybox");
	cubeMapConvolutionShaderProgram = &resourceManager->getShaderProgram("cubeMapConvolution");
	cubeMapPrefilterShaderProgram = &resourceManager->getShaderProgram("cubeMapPrefilter");
	brdfLutShaderProgram = &resourceManager->getShaderProgram("brdfLut");

	setInitialUniforms();
	environmentMap.renderToCubeMap(*environmentMapShaderProgram);
	environmentMap.convoluteCubeMap(*cubeMapConvolutionShaderProgram);
	environmentMap.prefilterCubeMap(*cubeMapPrefilterShaderProgram);
	environmentMap.createBrdfLUT(*brdfLutShaderProgram);
	renderContext->irradianceCubemap = environmentMap.irradianceCubeMapTexture;
	renderContext->prefilterCubemap = environmentMap.prefilterCubeMapTexture;
	renderContext->brdfLUTTexture = environmentMap.brdfLUTTexture;
}

void EnvironmentMapPass::execute()
{
	skyboxShaderProgram->use();
	const glm::mat4 viewMatrix = glm::mat4(glm::mat3(camera->getViewMatrix()));
	skyboxShaderProgram->setUniform("viewMatrix", viewMatrix);
	environmentMap.renderCubeMap();
}

void EnvironmentMapPass::setInitialUniforms() const
{
	const glm::mat4 projectionMatrix = renderContext->projectionMatrix;
	skyboxShaderProgram->use();
	skyboxShaderProgram->setUniform("projectionMatrix", projectionMatrix);
}
