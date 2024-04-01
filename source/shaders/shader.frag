#version 460 core

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_SPOTLIGHTS 4
#define MAX_POINTLIGHTS 4

#define MAX_DIRECTIONAL_LIGHTS_SHADOWS 2
#define MAX_SPOTLIGHTS_SHADOWS 2
#define MAX_POINTLIGHTS_SHADOWS 3

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec4 fragPosDirectionalLightSpace[MAX_DIRECTIONAL_LIGHTS_SHADOWS];
in vec4 fragPosSpotLightSpace[MAX_SPOTLIGHTS_SHADOWS];

uniform sampler2D texture_albedo1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_ao1;
uniform sampler2D texture_roughness1;
uniform sampler2D texture_metallic1;

uniform sampler2D ssaoBlurTexture;
uniform sampler2D depthTexture;

uniform samplerCube irradianceCubemap;
uniform samplerCube prefilteredEnvCubemap;
uniform sampler2D brdfLUT;

uniform vec3 ambientLightColor;
uniform float ambientLightIntensity;

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform sampler2D directionalLightDepthMaps[MAX_DIRECTIONAL_LIGHTS_SHADOWS];
uniform int directionalLightCount;
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
uniform sampler2D spotLightDepthMaps[MAX_SPOTLIGHTS_SHADOWS];
uniform int spotLightCount;
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
uniform samplerCube pointLightDepthCubemaps[MAX_POINTLIGHTS_SHADOWS];
uniform int pointLightCount;
uniform int shadowCastingPointLightsCount;

uniform float farPlane;
uniform vec3 viewPos;

in vec2 texCoord;
in vec3 fragPosTangent;
in vec3 viewPosTangent;
in vec3 fragPosWorldSpace;
in vec3 vertexNormal;
in vec4 fragPosClipSpace;

//in DirectionalLight directionalLightsTangent[MAX_DIRECTIONAL_LIGHTS];
//in SpotLight spotLightsTangent[MAX_SPOTLIGHTS];

in vec3 directionalLightsTangentDirections[MAX_DIRECTIONAL_LIGHTS];
in vec3 spotLightsTangentPositions[MAX_SPOTLIGHTS];
in vec3 spotLightsTangentDirections[MAX_SPOTLIGHTS];
in vec3 pointLightsTangentPositions[MAX_POINTLIGHTS];

float ambientOcclusionFactor;

struct LightGrid {
	uint offset;
	uint count;
};

layout(std430, binding = 2) buffer CompactClusters {
	uint compactClusters[];
};

layout(std140, binding = 3) buffer GlobalCount {
	uint activeClusterCount;
};

layout(std430, binding = 5) buffer LightGrids {
	LightGrid lightGrids[];
};

layout(std430, binding = 7) buffer LightIndices {
	uint lightIndices[];
};

uniform float zNear;
uniform float zFar;
uniform int numClustersZ;
uniform int numClustersX;
uniform int numClustersY;
uniform int clusterSizePx;
uniform vec2 screenDimensions;

// CLUSTERED LIGHT CULLING

uint getDepthSlice(float depth)
{
	return uint(floor(log(depth) * (numClustersZ / log(zFar / zNear)) - (numClustersZ * log(zNear) / log(zFar / zNear))));
}

float linearizeDepth(float z) {
//    float z_n = 2.0 * z - 1.0; // back to NDC
//    return (2.0 * zNear * zFar) / (zFar + zNear - z_n * (zFar - zNear));
	return (zNear * zFar) / (zFar + zNear - z * (zFar - zNear));
}

uint getClusterIndex(vec3 pixelCoord)
{
	float linearizedZ = linearizeDepth(pixelCoord.z);
	uint clusterZVal = getDepthSlice(float(linearizedZ));
	uvec3 cluster = uvec3(pixelCoord.xy / clusterSizePx, clusterZVal);
	return cluster.x + cluster.y * numClustersX + cluster.z * numClustersX * numClustersY;
}

// SHADOW MAPPING

float shadowCalculationDirectionalLight(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal, int textureIdx) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(directionalLightDepthMaps[textureIdx], projCoords.xy).r;
    float currentDepth = projCoords.z;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(directionalLightDepthMaps[textureIdx], 0);

	for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(directionalLightDepthMaps[textureIdx], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float shadowCalculationSpotLight(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal, int textureIdx) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

	float closestDepth = texture(spotLightDepthMaps[textureIdx], projCoords.xy).r;
    float currentDepth = projCoords.z;

	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(spotLightDepthMaps[textureIdx], 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(spotLightDepthMaps[textureIdx], projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1),
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float shadowCalculationPointLight(vec3 fragPos, vec3 lightPos, int cubemapIdx)
{
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPosTangent - fragPos);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointLightDepthCubemaps[cubemapIdx], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

// BLINN PHONG SHADING

vec3 calculateDirLightContribution(DirectionalLight directionalLight, vec3 albedoTex, vec3 specularTex, vec3 normal, vec3 normalV, vec3 viewDir, bool shadowCaster, int index)
{
	vec3 lightDir = normalize(-directionalLightsTangentDirections[index]);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWayDir), 0.0), 32);

	vec3 ambient = directionalLight.color * directionalLight.ambientIntensity * ambientOcclusionFactor;
	vec3 diffuse = diff * albedoTex * directionalLight.color * directionalLight.diffuseIntensity;
	vec3 specular = spec * specularTex * directionalLight.color * directionalLight.specularIntensity;

	vec3 result = vec3(0);
	if (shadowCaster)
	{
		float shadow = shadowCalculationDirectionalLight(fragPosDirectionalLightSpace[index], normalize(-directionalLight.direction), normalV, index);
		result = (ambient + (1.0 - shadow) * (diffuse + specular)) * directionalLight.color;
	}
	else
	{
		result = (ambient + diffuse + specular);
	}

	return result;
}

vec3 calculateSpotLightContribution(SpotLight spotLight, vec3 albedoTex, vec3 specularTex, vec3 normal, vec3 normalV, vec3 viewDir, vec3 fragPos, bool shadowCaster, int index)
{
	vec3 lightDir = normalize(spotLightsTangentPositions[index] - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWayDir), 0.0), 32);

	float theta = dot(lightDir, normalize(-spotLightsTangentDirections[index]));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = spotLight.color * spotLight.ambientIntensity * ambientOcclusionFactor;
	vec3 diffuse = diff * albedoTex * spotLight.color * spotLight.diffuseIntensity;
	vec3 specular = spec * specularTex * spotLight.color * spotLight.specularIntensity;

	float distance = length(spotLightsTangentPositions[index] - fragPos);
	float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * distance * distance);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	vec3 result = vec3(0);
	if (shadowCaster)
	{
		vec3 lightDir_ = normalize(spotLight.position - fragPosWorldSpace);
		float shadow = shadowCalculationSpotLight(fragPosSpotLightSpace[index], lightDir_, normalV, index);
		result = (ambient + (1.0 - shadow) * (diffuse + specular)) * spotLight.color;
	}
	else
	{
		result = (ambient + diffuse + specular);
	}

	return result;
}

vec3 calculatePointLightContribution(PointLight pointLight, vec3 albedoTex, vec3 specularTex, vec3 normal, vec3 viewDir, vec3 fragPos, bool shadowCaster, int index)
{
	vec3 lightDir = normalize(pointLightsTangentPositions[index] - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 halfWayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfWayDir), 0.0), 32);

	vec3 ambient = pointLight.color * pointLight.ambientIntensity * ambientOcclusionFactor;
	vec3 diffuse = diff * albedoTex * pointLight.color * pointLight.diffuseIntensity;
	vec3 specular = spec * specularTex * pointLight.color * pointLight.specularIntensity;

	float distance = length(pointLightsTangentPositions[index] - fragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = vec3(0);
	if (shadowCaster)
	{
		float shadow = shadowCalculationPointLight(fragPosWorldSpace, pointLight.position, index);
		result = (ambient + (1.0 - shadow) * (diffuse + specular)) * pointLight.color;
	}
	else
	{
		result = (ambient + diffuse + specular);
	}

	return result;
}

// PHYSICALLY BASED LIGHTING

const float PI = 3.14159265359;

float distributionGGX(vec3 normal, vec3 halfWayDir, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, halfWayDir), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometrySmith(vec3 normal, vec3 viewDir, vec3 lightDir, float roughness)
{
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{

//	for (int i = 0; i < directionalLightCount; i++)
//	{
//		if (i < shadowCastingDirectionalLightsCount)
//		{
//			result += calculateDirLightContribution(directionalLights[i], albedoTex, specularTex, normal, normalV, viewDir, true, i);
//		}
//		else
//		{
//			result += calculateDirLightContribution(directionalLights[i], albedoTex, specularTex, normal, normalV, viewDir, false, i);
//		}
//	}
//
//	for (int i = 0; i < spotLightCount; i++)
//	{
//		if (i < shadowCastingSpotLightsCount)
//		{
//			result += calculateSpotLightContribution(spotLights[i], albedoTex, specularTex, normal, normalV, viewDir, fragPosTangent, true, i);
//		}
//		else
//		{
//			result += calculateSpotLightContribution(spotLights[i], albedoTex, specularTex, normal, normalV, viewDir, fragPosTangent, false, i);
//		}
//	}

//	for (int i = 0; i < pointLightCount; i++)
//	{
//		if (i < shadowCastingPointLightsCount)
//		{
//			result += calculatePointLightContribution(pointLights[i], albedoTex, specularTex, normal, viewDir, fragPosTangent, true, i);
//		}
//		else
//		{
//			result += calculatePointLightContribution(pointLights[i], albedoTex, specularTex, normal, viewDir, fragPosTangent, false, i);
//		}
//	}

	vec2 screenSpaceCoords = (fragPosClipSpace.xy / fragPosClipSpace.w) * 0.5 + 0.5;
	ambientOcclusionFactor = texture(ssaoBlurTexture, screenSpaceCoords).r;
	vec2 fragPosScreen = screenSpaceCoords * screenDimensions;
	precise float depth = texture(depthTexture, screenSpaceCoords).r;
	uint clusterIndex = getClusterIndex(vec3(fragPosScreen.xy, depth));

	vec3 normalV = normalize(vertexNormal);
	vec3 normal = texture(texture_normal1, texCoord).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	vec3 viewDir = normalize(viewPosTangent - fragPosTangent);

	vec4 texColor = texture(texture_albedo1, texCoord);
	vec3 albedo = texColor.rgb;
	if (texColor.a < 0.1)
		discard;
	float metallic = texture(texture_metallic1, texCoord).r;
	float roughness = texture(texture_roughness1, texCoord).r;
	float ao = texture(texture_ao1, texCoord).r;

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0);

	for (int i = 0; i < lightGrids[clusterIndex].count; ++i)
	{
		uint lightIndex = lightIndices[lightGrids[clusterIndex].offset + i];

		vec3 lightDir = normalize(pointLightsTangentPositions[lightIndex] - fragPosTangent);
		vec3 halfWayDir = normalize(lightDir + viewDir);

		float distance = length(pointLightsTangentPositions[lightIndex] - fragPosTangent);
		float attenuation = 1.0 / (pointLights[lightIndex].constant + pointLights[lightIndex].linear * distance + pointLights[lightIndex].quadratic * (distance * distance));

		float shadow = shadowCalculationPointLight(fragPosWorldSpace, pointLights[lightIndex].position, int(lightIndex));
		vec3 radiance = pointLights[lightIndex].color * pointLights[lightIndex].diffuseIntensity * attenuation * (1.0 - shadow);

		float NDF = distributionGGX(normal, halfWayDir, roughness);
		float G = geometrySmith(normal, viewDir, lightDir, roughness);
		vec3 F = fresnelSchlick(max(dot(halfWayDir, viewDir), 0.0), F0);

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0) + 0.0001;
		vec3 specular = numerator / denominator;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		float NdotL = max(dot(normal, lightDir), 0.0);

		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	vec3 F = fresnelSchlickRoughness(max(dot(normal, viewDir), 0.0), F0, roughness);

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	vec3 irradiance = texture(irradianceCubemap, normalV).rgb;
	vec3 diffuse = irradiance * albedo;

	const float MAX_REFLECTION_LOD = 4.0;
	vec3 viewDirWorldSpace = normalize(viewPos - fragPosWorldSpace);
	vec3 prefilteredColor = textureLod(prefilteredEnvCubemap, reflect(-viewDirWorldSpace, normalV), roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfLUT, vec2(max(dot(normal, viewDir), 0.0), roughness)).rg;
	vec3 specular = prefilteredColor * (kS * brdf.x + brdf.y);

	vec3 ambient = (kD * diffuse + specular) * ao * ambientOcclusionFactor * ambientLightIntensity;

	vec3 color = ambient + Lo;
	fragColor = vec4(color, texColor.a);

	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = vec4(color, texColor.a);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}