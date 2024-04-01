#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, float speed): position(position), front(glm::normalize(front)), up(glm::normalize(up)), speed(speed)
{
	computeAngles();
	update();
}

void Camera::computeAngles()
{
	const glm::vec3 horizontalFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

	yaw = glm::degrees(acos(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), horizontalFront)));
	if (front.z < 0.0f)
		yaw = 360.0f - yaw;

	yaw = fmod(yaw, 360.0f);
	pitch = glm::degrees(asin(front.y));
}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

void Camera::moveForward(float deltaTime)
{
	position += front * speed * deltaTime;
}

void Camera::moveBackward(float deltaTime)
{
	position -= front * speed * deltaTime;
}

void Camera::moveLeft(float deltaTime)
{
	position -= right * speed * deltaTime;
}

void Camera::moveRight(float deltaTime)
{
	position += right * speed * deltaTime;
}

void Camera::moveUp(float deltaTime)
{
	position += up * speed * deltaTime;
}

void Camera::moveDown(float deltaTime)
{
	position -= up * speed * deltaTime;
}

void Camera::rotate(float xoffset, float yoffset, bool constrainPitch)
{
	yaw += xoffset * sensitivityYaw;
	yaw = fmod(yaw, 360.0f);

	pitch += yoffset * sensitivityPitch;

	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}

	update();
}

void Camera::lookAt(glm::vec3 target)
{
	front = glm::normalize(target - position);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
	computeAngles();
}

void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
	//up = glm::normalize(glm::cross(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)), front));
}
