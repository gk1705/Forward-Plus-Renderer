#pragma once
#include <glm/vec3.hpp>
#include <array>
#include <glm/fwd.hpp>

#include "ShadowCubemapPrerequisites.h"

class PointLight
{
public:
	PointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float constant, float linear,
		float quadratic, bool castsShadows);

	bool operator==(const PointLight& other) const;

	void cleanup() const;

	float getRadius() const;
	std::array<glm::mat4, 6> getShadowTransforms() const;

	void bindForWriting() const;

	glm::vec3 position;
	glm::vec3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;

	float constant;
	float linear;
	float quadratic;

	bool castsShadows = false;
	ShadowCubemapPrerequisites shadowCubemapPrerequisites;

private:
	constexpr static std::array<glm::vec3, 6> DIRECTIONS = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f)
	};

	constexpr static std::array<glm::vec3, 6> UP = {
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(0.0f, -1.0f, 0.0f)
	};

	constexpr static int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
};
