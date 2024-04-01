#pragma once

#include <random>
#include <vector>
#include <glm/vec3.hpp>

#include "IRenderPass.h"
#include "RenderContext.h"
#include "ResourceManager.h"

class SSAOPass : public IRenderPass
{
public:
	void setup() override;
	void execute() override;

private:
	std::uniform_real_distribution<float> distribution;
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	unsigned int noiseTexture;

	unsigned int ssaoFBO;
	unsigned int ssaoTexture;
	unsigned int ssaoBlurFBO;
	unsigned int ssaoBlurTexture;

	unsigned int depthTexture;

	RenderContext* renderContext = nullptr;
	ResourceManager* resourceManager = nullptr;

	ShaderProgram* ssaoShaderProgram = nullptr;
	ShaderProgram* ssaoBlurShaderProgram = nullptr;

	ScreenSpaceQuad* screenSpaceQuad = nullptr;

	void setupSSAO();
	void setInitialUniforms() const;
};
