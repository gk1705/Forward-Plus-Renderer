#pragma once

#include <glm/vec3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "ShadowMapPrerequisites.h"

inline bool epsilonEquals(float a, float b, float epsilon = 0.0001f)
{
	return std::abs(a - b) < epsilon;
}

class DirectionalLight
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;

	bool castsShadows = false;
	ShadowMapPrerequisites shadowMapPrerequisites;

	DirectionalLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float intensity, bool castsShadows);
	bool operator==(const DirectionalLight& other) const;

	void cleanup() const;

	void bindForWriting() const;
	glm::mat4 getLightSpaceMatrix() const;
};