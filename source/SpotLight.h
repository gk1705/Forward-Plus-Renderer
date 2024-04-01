#pragma once
#include <glm/fwd.hpp>
#include <glm/vec3.hpp>

#include "ShadowMapPrerequisites.h"

// todo: inherit from light class and make the shadow projective instead of orthographic
class SpotLight
{
public:
	SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float intensity,
		float constant, float linear, float quadratic, float cutOff, float outerCutOff, bool castsShadows);

	bool operator==(const SpotLight& other) const;

	void cleanup() const;

	void bindForWriting() const;

	glm::mat4 getLightSpaceMatrix() const;

	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 color;

	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;

	bool castsShadows = false;
	ShadowMapPrerequisites shadowMapPrerequisites;
};
