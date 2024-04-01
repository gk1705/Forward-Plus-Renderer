#include "SpotLight.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>

#include "DirectionalLight.h"

SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, float intensity,
                     float constant, float linear, float quadratic, float cutOff, float outerCutOff, bool castsShadows): position(position), direction(direction), color(color), ambientIntensity(0.1f), diffuseIntensity(intensity),
                                                                                                                         specularIntensity(intensity), constant(constant), linear(linear), quadratic(quadratic), cutOff(cutOff),
                                                                                                                         outerCutOff(outerCutOff), castsShadows(castsShadows)
{
	if (castsShadows)
	{
		shadowMapPrerequisites.init();
	}
}

bool SpotLight::operator==(const SpotLight& other) const
{
	return position == other.position && direction == other.direction && color == other.color &&
		ambientIntensity == other.ambientIntensity && diffuseIntensity == other.diffuseIntensity &&
		specularIntensity == other.specularIntensity && constant == other.constant && linear == other.linear &&
		quadratic == other.quadratic && cutOff == other.cutOff && outerCutOff == other.outerCutOff &&
		castsShadows == other.castsShadows;
}

void SpotLight::cleanup() const
{
	if (castsShadows)
	{
		glDeleteFramebuffers(1, &shadowMapPrerequisites.depthMapFBO);
		glDeleteTextures(1, &shadowMapPrerequisites.depthMapTexture);
	}
}

void SpotLight::bindForWriting() const
{
	if (castsShadows)
	{
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapPrerequisites.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

glm::mat4 SpotLight::getLightSpaceMatrix() const
{
	glm::vec3 upDirection;
	if (epsilonEquals(direction.y, 1.0f) || epsilonEquals(direction.y, -1.0f))
	{
		upDirection = glm::vec3(0.0f, 0.0f, -1.0f);
	}
	else
	{
		upDirection = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	return glm::ortho(shadowMapPrerequisites.left, shadowMapPrerequisites.right, shadowMapPrerequisites.bottom,
	                  shadowMapPrerequisites.top, shadowMapPrerequisites.zNear, shadowMapPrerequisites.zFar) * glm::lookAt(
		position, position + direction, upDirection);
}

