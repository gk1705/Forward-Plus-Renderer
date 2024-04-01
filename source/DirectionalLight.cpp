#include "DirectionalLight.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>

DirectionalLight::DirectionalLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color,
                                   float intensity, bool castsShadows): position(position), direction(direction), color(color), ambientIntensity(0.1f), diffuseIntensity(intensity), specularIntensity(intensity), castsShadows(castsShadows)
{
	if (castsShadows)
	{
		shadowMapPrerequisites.init();
	}
}

bool DirectionalLight::operator==(const DirectionalLight& other) const
{
	return position == other.position && direction == other.direction && color == other.color && ambientIntensity == other.ambientIntensity &&
		diffuseIntensity == other.diffuseIntensity && specularIntensity == other.specularIntensity && castsShadows == other.castsShadows;
}

void DirectionalLight::cleanup() const
{
	if (castsShadows)
	{
		glDeleteFramebuffers(1, &shadowMapPrerequisites.depthMapFBO);
		glDeleteTextures(1, &shadowMapPrerequisites.depthMapTexture);
	}
}

void DirectionalLight::bindForWriting() const
{
	if (castsShadows)
	{
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapPrerequisites.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

glm::mat4 DirectionalLight::getLightSpaceMatrix() const
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

