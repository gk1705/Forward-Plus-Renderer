#include "ResourceManager.h"

ShaderProgram& ResourceManager::getShaderProgram(const std::string& key)
{
	if (shaderPrograms.find(key) != shaderPrograms.end())
	{
		return shaderPrograms[key];
	}

	assert(false); // shader program not found
	return shaderPrograms["default"];
}

void ResourceManager::init()
{
	const Shader vertexShader("shaders/shader.vert", GL_VERTEX_SHADER);
	const Shader fragmentShader("shaders/shader.frag", GL_FRAGMENT_SHADER);

	const Shader outLineVertexShader("shaders/outline.vert", GL_VERTEX_SHADER);
	const Shader outLineFragmentShader("shaders/outline.frag", GL_FRAGMENT_SHADER);

	const Shader screenQuadVertexShader("shaders/screen_quad.vert", GL_VERTEX_SHADER);
	const Shader screenQuadFragmentShader("shaders/screen_quad.frag", GL_FRAGMENT_SHADER);

	const Shader skyboxVertexShader("shaders/skybox.vert", GL_VERTEX_SHADER);
	const Shader skyboxFragmentShader("shaders/skybox.frag", GL_FRAGMENT_SHADER);

	const Shader reflectiveVertexShader("shaders/reflective.vert", GL_VERTEX_SHADER);
	const Shader reflectiveFragmentShader("shaders/reflective.frag", GL_FRAGMENT_SHADER);

	const Shader depthMapVertexShader("shaders/depthMap.vert", GL_VERTEX_SHADER);
	const Shader depthMapFragmentShader("shaders/depthMap.frag", GL_FRAGMENT_SHADER);

	const Shader depthCubemapVertexShader("shaders/depthCubemap.vert", GL_VERTEX_SHADER);
	const Shader depthCubemapFragmentShader("shaders/depthCubemap.frag", GL_FRAGMENT_SHADER);
	const Shader depthCubemapGeometryShader("shaders/depthCubemap.geom", GL_GEOMETRY_SHADER);

	const Shader bloomVertexShader("shaders/bloom.vert", GL_VERTEX_SHADER);
	const Shader bloomFragmentShader("shaders/bloom.frag", GL_FRAGMENT_SHADER);

	const Shader depthPassVertexShader("shaders/depthPass.vert", GL_VERTEX_SHADER);
	const Shader depthPassFragmentShader("shaders/depthPass.frag", GL_FRAGMENT_SHADER);

	const Shader ssaoVertexShader("shaders/ssao.vert", GL_VERTEX_SHADER);
	const Shader ssaoFragmentShader("shaders/ssao.frag", GL_FRAGMENT_SHADER);

	const Shader ssaoBlurVertexShader("shaders/ssaoBlur.vert", GL_VERTEX_SHADER);
	const Shader ssaoBlurFragmentShader("shaders/ssaoBlur.frag", GL_FRAGMENT_SHADER);

	const Shader activeClusterSelectionComputeShader("shaders/activeClusterSelection.comp", GL_COMPUTE_SHADER);
	const Shader compactClustersComputeShader("shaders/compactClusters.comp", GL_COMPUTE_SHADER);
	const Shader resetClustersComputeShader("shaders/resetClusters.comp", GL_COMPUTE_SHADER);
	const Shader clusteredLightCullingComputeShader("shaders/clusteredLightCulling.comp", GL_COMPUTE_SHADER);

	const Shader environmentMapVertexShader("shaders/environmentMap.vert", GL_VERTEX_SHADER);
	const Shader environmentMapFragmentShader("shaders/environmentMap.frag", GL_FRAGMENT_SHADER);

	const Shader cubeMapConvolutionFragmentShader("shaders/cubemapConvolution.frag", GL_FRAGMENT_SHADER);

	const Shader cubeMapPrefilterFragmentShader("shaders/cubemapPrefilter.frag", GL_FRAGMENT_SHADER);

	const Shader brdfLutVertexShader("shaders/brdfLut.vert", GL_VERTEX_SHADER);
	const Shader brdfLutFragmentShader("shaders/brdfLut.frag", GL_FRAGMENT_SHADER);

	ShaderProgram defaultShaderProgramShaderProgram({ vertexShader, fragmentShader });
	shaderPrograms["default"] = defaultShaderProgramShaderProgram;
	ShaderProgram outLineShaderProgram({ outLineVertexShader, outLineFragmentShader });
	shaderPrograms["outline"] = outLineShaderProgram;
	ShaderProgram screenQuadShaderProgram({ screenQuadVertexShader, screenQuadFragmentShader });
	shaderPrograms["screenQuad"] = screenQuadShaderProgram;
	ShaderProgram skyboxShaderProgram({ skyboxVertexShader, skyboxFragmentShader });
	shaderPrograms["skybox"] = skyboxShaderProgram;
	ShaderProgram reflectiveShaderProgram({ reflectiveVertexShader, reflectiveFragmentShader });
	shaderPrograms["reflective"] = reflectiveShaderProgram;
	ShaderProgram depthMapShaderProgram({ depthMapVertexShader, depthMapFragmentShader });
	shaderPrograms["depthMap"] = depthMapShaderProgram;
	ShaderProgram depthCubeMapShaderProgram({ depthCubemapVertexShader, depthCubemapFragmentShader, depthCubemapGeometryShader });
	shaderPrograms["depthCubemap"] = depthCubeMapShaderProgram;
	ShaderProgram bloomShaderProgram({ bloomVertexShader, bloomFragmentShader });
	shaderPrograms["bloom"] = bloomShaderProgram;
	ShaderProgram depthPassShaderProgram({ depthPassVertexShader, depthPassFragmentShader });
	shaderPrograms["depthPass"] = depthPassShaderProgram;
	ShaderProgram ssaoShaderProgram({ ssaoVertexShader, ssaoFragmentShader });
	shaderPrograms["ssao"] = ssaoShaderProgram;
	ShaderProgram ssaoBlurShaderProgram({ ssaoBlurVertexShader, ssaoBlurFragmentShader });
	shaderPrograms["ssaoBlur"] = ssaoBlurShaderProgram;
	ShaderProgram activeClusterSelectionShaderProgram({ activeClusterSelectionComputeShader });
	shaderPrograms["activeClusterSelection"] = activeClusterSelectionShaderProgram;
	ShaderProgram compactClustersShaderProgram({ compactClustersComputeShader });
	shaderPrograms["compactClusters"] = compactClustersShaderProgram;
	ShaderProgram resetClustersShaderProgram({ resetClustersComputeShader });
	shaderPrograms["resetClusters"] = resetClustersShaderProgram;
	ShaderProgram clusteredLightCullingShaderProgram({ clusteredLightCullingComputeShader });
	shaderPrograms["clusteredLightCulling"] = clusteredLightCullingShaderProgram;
	ShaderProgram environmentMapShaderProgram({ environmentMapVertexShader, environmentMapFragmentShader });
	shaderPrograms["environmentMap"] = environmentMapShaderProgram;
	ShaderProgram cubeMapConvolutionShaderProgram({ environmentMapVertexShader, cubeMapConvolutionFragmentShader });
	shaderPrograms["cubeMapConvolution"] = cubeMapConvolutionShaderProgram;
	ShaderProgram cubeMapPrefilterShaderProgram({ environmentMapVertexShader, cubeMapPrefilterFragmentShader });
	shaderPrograms["cubeMapPrefilter"] = cubeMapPrefilterShaderProgram;
	ShaderProgram brdfLutShaderProgram({ brdfLutVertexShader, brdfLutFragmentShader });
	shaderPrograms["brdfLut"] = brdfLutShaderProgram;
}

void ResourceManager::cleanup() const
{
	for (auto& shaderProgram : shaderPrograms)
	{
		shaderProgram.second.cleanup();
	}
}
