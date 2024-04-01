#pragma once

#include <vector>
#include <glm/vec4.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "RenderContext.h"

#include "ShaderProgram.h"
#include "Singleton.h"
#include "SpotLight.h"

class LightManager : public Singleton<LightManager>
{
	friend class Singleton<LightManager>;

public:
	std::vector<PointLight> pointLights;

	void init();
	void cleanup() const;

	void addDirectionalLight(const DirectionalLight& light);
	void addSpotLight(const SpotLight& light);
	void addPointLight(const PointLight& light);

	void removeDirectionalLight(const DirectionalLight& light);
	void removeSpotLight(const SpotLight& light);
	void removePointLight(const PointLight& light);

	// TODO: combine classes for depth maps and cubemaps
	void depthPassTextures(ShaderProgram& shaderProgram) const;
	void depthPassCubemaps(ShaderProgram& depthCubeMapShaderProgram) const;
	void setLightUniforms(ShaderProgram& shaderProgram) const;

protected:
	LightManager() = default;

private:
	constexpr static unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	std::vector<DirectionalLight> directionalLights;
	std::vector<SpotLight> spotLights;

	glm::vec3 ambientLightColor;
	float ambientLightIntensity;

	RenderContext* renderContext = nullptr;

	static constexpr int MAX_DIRECTIONAL_LIGHT_SHADOWS = 2;
	static constexpr int MAX_SPOTLIGHTS = 2;
	static constexpr int MAX_POINTLIGHT_SHADOWS = 3;
};
