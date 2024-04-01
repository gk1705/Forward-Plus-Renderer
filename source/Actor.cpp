#include "Actor.h"

Actor::Actor(Model& model, const glm::mat4& modelMatrix): modelMatrix(modelMatrix), model(model)
{
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

Actor& Actor::operator=(const Actor& other)
{
	if (this != &other)
	{
		modelMatrix = other.modelMatrix;
		normalMatrix = other.normalMatrix;
		model = other.model;
		isOutlined = other.isOutlined;
	}
	return *this;
}

Actor& Actor::operator=(Actor&& other) noexcept
{
	if (this != &other)
	{
		modelMatrix = other.modelMatrix;
		normalMatrix = other.normalMatrix;
		model = other.model;
		isOutlined = other.isOutlined;
	}
	return *this;
}

Actor::Actor(Model& model): modelMatrix(glm::mat4(1.0f)), model(model)
{
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

void Actor::draw(ShaderProgram& shader) const
{
	shader.use();
	shader.setUniform(std::string("modelMatrix"), modelMatrix);
	shader.setUniform(std::string("normalMatrix"), normalMatrix);
	model.draw(shader);
}

void Actor::translate(const glm::vec3& translation)
{
	modelMatrix = glm::translate(modelMatrix, translation);
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

void Actor::rotate(float angle, const glm::vec3& axis)
{
	modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), axis);
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}

void Actor::scale(const glm::vec3& scale)
{
	modelMatrix = glm::scale(modelMatrix, scale);
	normalMatrix = glm::transpose(glm::inverse(modelMatrix));
}
