#include "ShadowPass.h"

#include "LightManager.h"

void ShadowPass::setup()
{
	depthMapShaderProgram = &ResourceManager::getInstance().getShaderProgram("depthMap");
	depthCubeMapShaderProgram = &ResourceManager::getInstance().getShaderProgram("depthCubemap");
}

void ShadowPass::execute()
{
	if (depthMapShaderProgram == nullptr || depthCubeMapShaderProgram == nullptr) assert(false);

	LightManager& lightManager = LightManager::getInstance();
	glEnable(GL_CULL_FACE);
	lightManager.depthPassTextures(*depthMapShaderProgram);
	lightManager.depthPassCubemaps(*depthCubeMapShaderProgram);
	glDisable(GL_CULL_FACE);
}
