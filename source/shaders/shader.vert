#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTexCoord;

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_DIRECTIONAL_LIGHTS_SHADOWS 2

#define MAX_SPOTLIGHTS 4
#define MAX_SPOTLIGHTS_SHADOWS 2

#define MAX_POINTLIGHTS 4

out vec4 fragPosDirectionalLightSpace[MAX_DIRECTIONAL_LIGHTS_SHADOWS];
out vec4 fragPosSpotLightSpace[MAX_SPOTLIGHTS_SHADOWS];
uniform mat4 directionalLightSpaceMatrices[MAX_DIRECTIONAL_LIGHTS_SHADOWS];
uniform mat4 spotLightSpaceMatrices[MAX_SPOTLIGHTS_SHADOWS];

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
out vec3 directionalLightsTangentDirections[MAX_DIRECTIONAL_LIGHTS];
uniform int shadowCastingDirectionalLightsCount;

struct SpotLight {
	vec3 color;
	vec3 position;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};
uniform SpotLight spotLights[MAX_SPOTLIGHTS];
out vec3 spotLightsTangentPositions[MAX_SPOTLIGHTS];
out vec3 spotLightsTangentDirections[MAX_SPOTLIGHTS];
uniform int shadowCastingSpotLightsCount;

struct PointLight
{
	vec3 color;
	vec3 position;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
	float constant;
	float linear;
	float quadratic;
};
uniform PointLight pointLights[MAX_POINTLIGHTS];
out vec3 pointLightsTangentPositions[MAX_POINTLIGHTS];
uniform int shadowCastingPointLightsCount;

out vec2 texCoord;
out vec3 fragPosTangent;
out vec3 viewPosTangent;
out vec3 fragPosWorldSpace;
out vec3 vertexNormal;
out vec4 fragPosClipSpace;

uniform vec3 viewPos;

void main()
{
	vec3 fragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	texCoord = aTexCoord;

	vec3 T = normalize(mat3(normalMatrix) * aTangent);
	vec3 N = normalize(mat3(normalMatrix) * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	mat3 TBN = transpose(mat3(T, B, N));
	fragPosTangent = TBN * fragPos;
	viewPosTangent = TBN * viewPos;

	fragPosWorldSpace = fragPos;
	vertexNormal = mat3(normalMatrix) * aNormal;

	for (int i = 0; i < shadowCastingDirectionalLightsCount; i++)
	{
		directionalLightsTangentDirections[i] = TBN * directionalLights[i].direction;
	}

	for (int i = 0; i < shadowCastingSpotLightsCount; i++)
	{
		spotLightsTangentPositions[i] = TBN * spotLights[i].position;
		spotLightsTangentDirections[i] = TBN * spotLights[i].direction;
	}

	for (int i = 0; i < shadowCastingPointLightsCount; i++)
	{
		pointLightsTangentPositions[i] = TBN * pointLights[i].position;
	}

	for (int i = 0; i < shadowCastingDirectionalLightsCount; i++)
	{
		fragPosDirectionalLightSpace[i] = directionalLightSpaceMatrices[i] * vec4(fragPos, 1.0);
	}
	for (int i = 0; i < shadowCastingSpotLightsCount; i++)
	{
		fragPosSpotLightSpace[i] = spotLightSpaceMatrices[i] * vec4(fragPos, 1.0);
	}

	fragPosClipSpace = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
	gl_Position = fragPosClipSpace;
}