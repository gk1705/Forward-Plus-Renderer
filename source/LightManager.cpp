#include "LightManager.h"

void LightManager::init()
{
	renderContext = &RenderContext::getInstance();

	glm::vec3 dir_ = glm::normalize(glm::vec3(0.f, -1.0f, 0.0f));
	DirectionalLight directionalLight(glm::vec3(0.0f, 5.0f, 0.0f), dir_, glm::vec3(1.0f, 1.0f, 1.0f), 1.7f, true);
	directionalLight.shadowMapPrerequisites.left = -10.0f;
	directionalLight.shadowMapPrerequisites.right = 10.0f;
	directionalLight.shadowMapPrerequisites.bottom = -10.0f;
	directionalLight.shadowMapPrerequisites.top = 10.0f;
	directionalLight.shadowMapPrerequisites.zNear = 1.0f;
	directionalLight.shadowMapPrerequisites.zFar = 100.0f;

	glm::vec3 dir = glm::normalize(glm::vec3(-0.5f, 0.0f, 0.5f));
	SpotLight spotLight(glm::vec3(-0.5f, 0.0f, 1.0f), -dir, glm::vec3(1.0f, 1.0f, 1.0f),
	                    1.0f, 1.0f, 0.02f, 0.012f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), true);
	spotLight.shadowMapPrerequisites.left = -10.0f;
	spotLight.shadowMapPrerequisites.right = 10.0f;
	spotLight.shadowMapPrerequisites.bottom = -10.0f;
	spotLight.shadowMapPrerequisites.top = 10.0f;
	spotLight.shadowMapPrerequisites.zNear = 1.0f;
	spotLight.shadowMapPrerequisites.zFar = 100.0f;

	PointLight pointLight(glm::vec3(0.0f, 3.5f, -1.5f), glm::vec3(1.0f, 1.0f, 1.0f), 1.f, 1.0f, 0.02f, 0.012f, true);
	pointLight.shadowCubemapPrerequisites.nearP = 1.0f;
	pointLight.shadowCubemapPrerequisites.farP = 25.0f;

	PointLight pointLight2(glm::vec3(2.0f, 0.5f, 1.5f), glm::vec3(1.0f, 1.0f, 1.0f), 1.f, 1.0f, 0.02f, 0.012f, true);
	pointLight2.shadowCubemapPrerequisites.nearP = 1.0f;
	pointLight2.shadowCubemapPrerequisites.farP = 25.0f;

	//lightManager.addDirectionalLight(directionalLight);
	//lightManager.addSpotLight(spotLight);
	addPointLight(pointLight);
	addPointLight(pointLight2);

	ambientLightColor = glm::vec3(1.0f);
	ambientLightIntensity = 0.2f;
}

void LightManager::cleanup() const
{
	for (auto& light : directionalLights)
	{
		light.cleanup();
	}

	for (auto& light : spotLights)
	{
		light.cleanup();
	}

	for (auto& light : pointLights)
	{
		light.cleanup();
	}
}

void LightManager::addDirectionalLight(const DirectionalLight& light)
{
	directionalLights.push_back(light);

	std::sort(directionalLights.begin(), directionalLights.end(), [](const DirectionalLight& a, const DirectionalLight& b)
	{
		return a.castsShadows > b.castsShadows;
	});
}

void LightManager::addSpotLight(const SpotLight& light)
{
	spotLights.push_back(light);

	std::sort(spotLights.begin(), spotLights.end(), [](const SpotLight& a, const SpotLight& b)
	{
		return a.castsShadows > b.castsShadows;
	});
}

void LightManager::addPointLight(const PointLight& light)
{
	pointLights.push_back(light);

	std::sort(pointLights.begin(), pointLights.end(), [](const PointLight& a, const PointLight& b)
	{
		return a.castsShadows > b.castsShadows;
	});
}

void LightManager::removeDirectionalLight(const DirectionalLight& light)
{
	directionalLights.erase(std::remove(directionalLights.begin(), directionalLights.end(), light), directionalLights.end());
	light.cleanup();
}

void LightManager::removeSpotLight(const SpotLight& light)
{
	spotLights.erase(std::remove(spotLights.begin(), spotLights.end(), light), spotLights.end());
	light.cleanup();
}

void LightManager::removePointLight(const PointLight& light)
{
	pointLights.erase(std::remove(pointLights.begin(), pointLights.end(), light), pointLights.end());
	light.cleanup();
}

void LightManager::depthPassTextures(ShaderProgram& shaderProgram) const
{
	for (const auto& light : directionalLights)
	{
		if (light.castsShadows)
		{
			light.bindForWriting();
			shaderProgram.use();

			const glm::mat4 lightSpaceMatrix = light.getLightSpaceMatrix();
			shaderProgram.setUniform("lightSpaceMatrix", lightSpaceMatrix);

			renderContext->renderAll(shaderProgram);
		}
	}

	for (const auto& light : spotLights)
	{
		if (light.castsShadows)
		{
			light.bindForWriting();
			shaderProgram.use();

			const glm::mat4 lightSpaceMatrix = light.getLightSpaceMatrix();
			shaderProgram.setUniform("lightSpaceMatrix", lightSpaceMatrix);

			renderContext->renderAll(shaderProgram);
		}
	}
}

void LightManager::depthPassCubemaps(ShaderProgram& depthCubeMapShaderProgram) const
{
	for (const auto& light : pointLights)
	{
		if (light.castsShadows)
		{
			light.bindForWriting();
			depthCubeMapShaderProgram.use();

			std::array<glm::mat4, 6> shadowTransforms = light.getShadowTransforms();
			for (unsigned int i = 0; i < 6; ++i)
			{
				depthCubeMapShaderProgram.setUniform("shadowTransforms[" + std::to_string(i) + "]", shadowTransforms[i]);
			}
			depthCubeMapShaderProgram.setUniform("farPlane", light.shadowCubemapPrerequisites.farP);
			depthCubeMapShaderProgram.setUniform("lightPos", light.position);

			renderContext->renderAll(depthCubeMapShaderProgram);
		}
	}
}

void LightManager::setLightUniforms(ShaderProgram& shaderProgram) const
{
	shaderProgram.use();
	shaderProgram.setUniform("ambientLightColor", ambientLightColor);
	shaderProgram.setUniform("ambientLightIntensity", ambientLightIntensity);

	int shadowCastingDirectionalLightsCount = 0;
	int textureUnitIndex = 10;
	for (const auto& directionalLight : directionalLights)
	{
		if (directionalLight.castsShadows)
		{
			const glm::mat4 lightSpaceMatrix = directionalLight.getLightSpaceMatrix();
			shaderProgram.setUniform("directionalLightSpaceMatrices[" + std::to_string(shadowCastingDirectionalLightsCount) + "]", lightSpaceMatrix);
			shaderProgram.setLight(directionalLight);
			glActiveTexture(GL_TEXTURE0 + textureUnitIndex);
			shaderProgram.setUniform("directionalLightDepthMaps[" + std::to_string(shadowCastingDirectionalLightsCount) + "]", textureUnitIndex);
			glBindTexture(GL_TEXTURE_2D, directionalLight.shadowMapPrerequisites.depthMapTexture);
			textureUnitIndex++;
			shadowCastingDirectionalLightsCount++;
		}
	}
	shaderProgram.setUniform("shadowCastingDirectionalLightsCount", shadowCastingDirectionalLightsCount);

	int shadowCastingSpotLightsCount = 0;
	for (const auto& spotLight : spotLights)
	{
		if (spotLight.castsShadows)
		{
			const glm::mat4 lightSpaceMatrix = spotLight.getLightSpaceMatrix();
			shaderProgram.setUniform("spotLightSpaceMatrices[" + std::to_string(shadowCastingSpotLightsCount) + "]", lightSpaceMatrix);
			shaderProgram.setLight(spotLight);
			glActiveTexture(GL_TEXTURE0 + textureUnitIndex);
			shaderProgram.setUniform("spotLightDepthMaps[" + std::to_string(shadowCastingSpotLightsCount) + "]", textureUnitIndex);
			glBindTexture(GL_TEXTURE_2D, spotLight.shadowMapPrerequisites.depthMapTexture);
			textureUnitIndex++;
			shadowCastingSpotLightsCount++;
		}
	}
	shaderProgram.setUniform("shadowCastingSpotLightsCount", shadowCastingSpotLightsCount);

	int shadowCastingPointLightsCount = 0;
	//TODO: multiple far plane values in the shader?
	shaderProgram.setUniform("farPlane", 25.f);
	for (const auto& pointLight : pointLights)
	{
		if (pointLight.castsShadows)
		{
			shaderProgram.setLight(pointLight);
			glActiveTexture(GL_TEXTURE0 + textureUnitIndex);
			shaderProgram.setUniform("pointLightDepthCubemaps[" + std::to_string(shadowCastingPointLightsCount) + "]", textureUnitIndex);
			glBindTexture(GL_TEXTURE_CUBE_MAP, pointLight.shadowCubemapPrerequisites.depthMapCubemap);
			textureUnitIndex++;
			shadowCastingPointLightsCount++;
		}
	}
	shaderProgram.setUniform("shadowCastingPointLightsCount", shadowCastingPointLightsCount);

	// bind remaining indices so sampler arrays aren't all pointing to location 0, otherwise openGL will complain
	while (shadowCastingDirectionalLightsCount < MAX_DIRECTIONAL_LIGHT_SHADOWS)
	{
		shaderProgram.setUniform("directionalLightDepthMaps[" + std::to_string(shadowCastingDirectionalLightsCount) + "]", textureUnitIndex);
		textureUnitIndex++;
		shadowCastingDirectionalLightsCount++;
	}

	while (shadowCastingSpotLightsCount < MAX_SPOTLIGHTS)
	{
		shaderProgram.setUniform("spotLightDepthMaps[" + std::to_string(shadowCastingSpotLightsCount) + "]", textureUnitIndex);
		textureUnitIndex++;
		shadowCastingSpotLightsCount++;
	}

	while (shadowCastingPointLightsCount < MAX_POINTLIGHT_SHADOWS)
	{
		shaderProgram.setUniform("pointLightDepthCubemaps[" + std::to_string(shadowCastingPointLightsCount) + "]", textureUnitIndex);
		textureUnitIndex++;
		shadowCastingPointLightsCount++;
	}
}
