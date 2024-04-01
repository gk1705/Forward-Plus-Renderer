#include "Application.h"

#include <fstream>

#include "BloomPass.h"
#include "DepthPrePass.h"
#include "EnvironmentMapPass.h"
#include "LightCullingPass.h"
#include "PostProcessingPass.h"
#include "RenderingPassFactory.h"
#include "ShadowPass.h"
#include "SSAOPass.h"

void Application::init()
{
	window = nullptr;
	if (initWindow() || !window)
	{
		throw std::runtime_error("Failed to initialize GLFW");
	}

	camera = Camera(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 10.0f);
	clusterGrid = ClusterGrid(0.1f, 100.f, glm::vec4(16, 9, 24, std::ceilf(WIDTH / 16.f)), glm::vec2(WIDTH, HEIGHT));
	screenSpaceQuad.init();

	ResourceManager::getInstance().init();

	RenderContext& renderContext = RenderContext::getInstance();
	renderContext.camera = &camera;
	renderContext.clusterGrid = &clusterGrid;
	renderContext.screenSpaceQuad = &screenSpaceQuad;

	renderContext.init();
	LightManager::getInstance().init();

	clusterGrid.init(glm::inverse(renderContext.projectionMatrix));

	DepthPrePass* depthPrePass = RenderingPassFactory::createRenderingPass<DepthPrePass>(RenderPassType::DepthPrePass);
	ShadowPass* shadowPass = RenderingPassFactory::createRenderingPass<ShadowPass>(RenderPassType::ShadowPass);
	SSAOPass* ssaoPass = RenderingPassFactory::createRenderingPass<SSAOPass>(RenderPassType::SSAOPass);
	LightCullingPass* lightCullingPass = RenderingPassFactory::createRenderingPass<LightCullingPass>(RenderPassType::LightCullingPass);
	MainRenderPass* mainRenderPass = RenderingPassFactory::createRenderingPass<MainRenderPass>(RenderPassType::MainRenderPass);
	EnvironmentMapPass* environmentMapPass = RenderingPassFactory::createRenderingPass<EnvironmentMapPass>(RenderPassType::EnvironmentMapPass);
	BloomPass* bloomPass = RenderingPassFactory::createRenderingPass<BloomPass>(RenderPassType::BloomPass);
	PostProcessingPass* postProcessingPass = RenderingPassFactory::createRenderingPass<PostProcessingPass>(RenderPassType::PostProcessingPass);

	renderingPipelineManager.addRenderingPass(depthPrePass);
	renderingPipelineManager.addRenderingPass(shadowPass);
	renderingPipelineManager.addRenderingPass(ssaoPass);
	renderingPipelineManager.addRenderingPass(lightCullingPass);
	renderingPipelineManager.addRenderingPass(mainRenderPass);
	renderingPipelineManager.addRenderingPass(environmentMapPass);
	renderingPipelineManager.addRenderingPass(bloomPass);
	renderingPipelineManager.addRenderingPass(postProcessingPass);
}

void Application::cleanup() const
{
	ResourceManager::getInstance().cleanup();
	RenderContext::getInstance().cleanup();
	LightManager::getInstance().cleanup();
	RenderingPassFactory::clearRenderingPasses();
	screenSpaceQuad.cleanup();
	clusterGrid.cleanup();
}

void Application::run()
{
	double lastTime = glfwGetTime();
	constexpr double maxFPS = 60.0;
	constexpr double maxPeriod = 1.0 / maxFPS;

	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		if (deltaTime < maxPeriod)
		{
			int sleepTime = static_cast<int>((maxPeriod - deltaTime) * 1000.0);
			std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
			currentTime = glfwGetTime();
			deltaTime = currentTime - lastTime;
		}

		lastTime = currentTime;
		processInput(deltaTime);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);

		renderingPipelineManager.executePipeline();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int Application::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Slicer", NULL, NULL);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetWindowUserPointer(window, this);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glViewport(0, 0, WIDTH, HEIGHT);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	return 0;
}

void Application::processInput(float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.moveForward(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.moveBackward(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.moveLeft(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.moveRight(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		camera.moveUp(deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		camera.moveDown(deltaTime);
	}

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		camera.lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
	}
}

std::vector<char> Application::readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate);
	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}
	const size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);
	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	return buffer;
}

unsigned Application::generateShader(const std::string& filePath, GLenum shaderType)
{
	const std::vector<char> shaderCode = readFile(filePath);
	const char* shaderCodePtr = shaderCode.data();
	const unsigned int shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderCodePtr, NULL);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return shader;
}

void Application::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	const GLchar* message, const void* userParam)
{
	// ignore non-significant error/warning codes, comment that out if you want to see those, but opengl just floods the console with them
	/*if (severity != GL_DEBUG_SEVERITY_HIGH || severity != GL_DEBUG_SEVERITY_MEDIUM)
		{
			return;
		}*/

	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
	        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
	        type, severity, message);
}

void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void Application::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	static float lastX = 0.0f;
	static float lastY = 0.0f;
	static bool firstMouse = true;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
	app->camera.rotate(xoffset, yoffset);
}

/*static void APIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		std::cout << "---------------------opengl-callback-start------------" << std::endl;
		std::cout << "message: " << message << std::endl;
		std::cout << "type: ";
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
			std::cout << "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			std::cout << "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			std::cout << "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			std::cout << "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			std::cout << "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			std::cout << "OTHER";
			break;
		}
		std::cout << std::endl;

		std::cout << "id: " << id << std::endl;
		std::cout << "severity: ";
		switch (severity)
		{
		case GL_DEBUG_SEVERITY_LOW:
			std::cout << "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			std::cout << "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			std::cout << "HIGH";
			break;
		}
		std::cout << std::endl;
		std::cout << "---------------------opengl-callback-end--------------" << std::endl;
	}*/

