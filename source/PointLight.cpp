#include "PointLight.h"

#include <glad/glad.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float intensity, float constant, float linear,
                       float quadratic, bool castsShadows): position(position), color(color), ambientIntensity(0.1f), diffuseIntensity(intensity),
                                                            specularIntensity(intensity), constant(constant), linear(linear), quadratic(quadratic), castsShadows(castsShadows)
{
	if (castsShadows)
	{
		shadowCubemapPrerequisites.init();
	}
}

bool PointLight::operator==(const PointLight& other) const
{
	return position == other.position && color == other.color && ambientIntensity == other.ambientIntensity &&
		diffuseIntensity == other.diffuseIntensity && specularIntensity == other.specularIntensity &&
		constant == other.constant && linear == other.linear && quadratic == other.quadratic &&
		castsShadows == other.castsShadows;
}

float PointLight::getRadius() const
{
	const float maxIntensity = std::max(std::max(color.r, color.g), color.b);
	const float lightMax = std::fmaxf(std::fmaxf(color.r, color.g), color.b);
	const float lightMaxIntensity = std::fmaxf(std::fmaxf(ambientIntensity, diffuseIntensity), specularIntensity);
	const float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - 256.0f / 5.0f * maxIntensity))) / (2 * quadratic);
	return radius;
}

void PointLight::cleanup() const
{
	if (castsShadows)
	{
		glDeleteFramebuffers(1, &shadowCubemapPrerequisites.depthMapFBO);
		glDeleteTextures(1, &shadowCubemapPrerequisites.depthMapCubemap);
	}
}

void PointLight::bindForWriting() const
{
	if (castsShadows)
	{
		glViewport(0, 0, 1024, 1024);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowCubemapPrerequisites.depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
}

std::array<glm::mat4, 6> PointLight::getShadowTransforms() const
{
	constexpr float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;

	std::array<glm::mat4, 6> shadowTransforms;
	const glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, shadowCubemapPrerequisites.nearP, shadowCubemapPrerequisites.farP);
	for (unsigned int i = 0; i < 6; ++i)
	{
		shadowTransforms[i] = shadowProj * glm::lookAt(position, position + DIRECTIONS[i], UP[i]);
	}
	return shadowTransforms;
}

