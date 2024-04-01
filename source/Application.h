#pragma once
#include <random>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Skybox.h"
#include "MainRenderPass.h"
#include "RenderingPipelineManager.h"
#include "ScreenSpaceQuad.h"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

class Application
{
public:
	GLFWwindow* window = nullptr;

	RenderingPipelineManager renderingPipelineManager;

	Camera camera;
	ClusterGrid clusterGrid;
	ScreenSpaceQuad screenSpaceQuad;

	void init();
	void cleanup() const;

	~Application()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void run();

private:
	int initWindow();
	void processInput(float deltaTime);

	// HELPER FUNCTIONS
	static std::vector<char> readFile(const std::string& filename);
	static unsigned int generateShader(const std::string& filePath, GLenum shaderType);

	// CALLBACKS
	static void GLAPIENTRY
		MessageCallback(GLenum source,
			GLenum type,
			GLuint id,
			GLenum severity,
			GLsizei length,
			const GLchar* message,
			const void* userParam);

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
	static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
};
