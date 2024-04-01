#pragma once

#include "Model.h"
#include "ShaderProgram.h"

class Actor
{
public:
	Actor(Model& model, const glm::mat4& modelMatrix);

	Actor(const Actor& other) : modelMatrix(other.modelMatrix), normalMatrix(other.normalMatrix), model(other.model), isOutlined(other.isOutlined) {}
	Actor& operator=(const Actor& other);
	Actor(Actor&& other) noexcept : modelMatrix(other.modelMatrix), normalMatrix(other.normalMatrix), model(other.model), isOutlined(other.isOutlined) {}
	Actor& operator=(Actor&& other) noexcept;

	explicit Actor(Model& model);

	void draw(ShaderProgram& shader) const;

	void translate(const glm::vec3& translation);
	void rotate(float angle, const glm::vec3& axis);
	void scale(const glm::vec3& scale);

	glm::mat4 modelMatrix{};
	glm::mat4 normalMatrix{};
	Model& model;
	bool isOutlined = false;
};
