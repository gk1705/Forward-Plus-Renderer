#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Camera
{
public:
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	float speed;

	float yaw;
	float pitch;

	Camera() = default;
	Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float speed);

	glm::mat4 getViewMatrix() const;

	void moveForward(float deltaTime);
	void moveBackward(float deltaTime);
	void moveLeft(float deltaTime);
	void moveRight(float deltaTime);
	void moveUp(float deltaTime);
	void moveDown(float deltaTime);

	void rotate(float xoffset, float yoffset, bool constrainPitch = true);
	void lookAt(glm::vec3 target);

private:
	void computeAngles();
	void update();

	float sensitivityYaw = 0.1f;
	float sensitivityPitch = 0.1f;

	constexpr static glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
};