#pragma once

#include "ScreenSpaceQuad.h"
#include "Singleton.h"
#include "Model.h"
#include "Actor.h"
#include <vector>

class Camera;
class ClusterGrid;

class RenderContext : public Singleton<RenderContext>
{
	friend class Singleton<RenderContext>;
public:
	int WIDTH = 1280;
	int HEIGHT = 720;
	int MSAA = 4;
	glm::mat4 projectionMatrix;

	Camera* camera = nullptr;
	ClusterGrid* clusterGrid = nullptr;
	ScreenSpaceQuad* screenSpaceQuad = nullptr;

	std::vector<Model> models;
	std::vector<Actor> actors;

	unsigned int frameBuffer;
	unsigned int frameBufferTexture;
	unsigned int renderBuffer;

	unsigned int depthFBOMultisample;
	unsigned int depthTextureMultisampled;
	unsigned int depthFBO;
	unsigned int depthTexture;

	unsigned int intermediateFBO;
	unsigned int screenTexture;

	unsigned int bloomFBO;
	unsigned int bloomTextureMultisampled;
	unsigned int bloomTexture;

	unsigned int pingPongFBOs[2];
	unsigned int pingPongTextures[2];
	bool horizontal = true;

	unsigned int noiseTexture;

	unsigned int ssaoFBO;
	unsigned int ssaoTexture;

	unsigned int ssaoBlurFBO;
	unsigned int ssaoBlurTexture;

	unsigned int irradianceCubemap;
	unsigned int prefilterCubemap;
	unsigned int brdfLUTTexture;

	//static std::vector<Actor> billboardActors;
	//static std::vector<Actor> transparentActors;
	//static std::vector<Actor> reflectiveActors;

	void init();
	void cleanup() const;

	void loadModels();
	void createActors();
	void setupFrameBuffersAndTextures();

	void renderNonOutlinedActors(ShaderProgram& shaderProgram) const;
	void renderOutlinedActors(ShaderProgram& shaderProgram) const;
	void renderAll(ShaderProgram& shaderProgram) const;

protected:
	RenderContext() = default;
};
