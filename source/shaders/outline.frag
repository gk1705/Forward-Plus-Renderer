#version 460 core

out vec4 fragColor;

in vec3 fragPos;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

#define MAX_DIRECTIONAL_LIGHTS 5
struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float ambientIntensity;
	float diffuseIntensity;
	float specularIntensity;
};
uniform DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
uniform int directionalLightCount;

#define MAX_POINT_LIGHTS 20
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
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightCount;

#define MAX_SPOT_LIGHTS 20
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
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int spotLightCount;

uniform vec3 viewPos;

uniform vec3 outlineColor;

vec3 calculateDirLightContribution(DirectionalLight directionalLight, vec3 albedoTex, vec3 specularTex, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-directionalLight.direction);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambient = directionalLight.color * directionalLight.ambientIntensity;
	vec3 diffuse = diff * albedoTex * directionalLight.color * directionalLight.diffuseIntensity;
	vec3 specular = spec * specularTex * directionalLight.color * directionalLight.specularIntensity;

	vec3 result = (ambient + diffuse + specular);
	return result;
}

vec3 calculatePointLightContribution(PointLight pointLight, vec3 albedoTex, vec3 specularTex, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(pointLight.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	vec3 ambient = pointLight.color * pointLight.ambientIntensity;
	vec3 diffuse = diff * albedoTex * pointLight.color * pointLight.diffuseIntensity;
	vec3 specular = spec * specularTex * pointLight.color * pointLight.specularIntensity;

	float distance = length(pointLight.position - fragPos);
	float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = (ambient + diffuse + specular);
	return result;
}

vec3 calculateSpotLigthContribution(SpotLight spotLight, vec3 albedoTex, vec3 specularTex, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(spotLight.position - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = spotLight.cutOff - spotLight.outerCutOff;
	float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);

	vec3 ambient = spotLight.color * spotLight.ambientIntensity;
	vec3 diffuse = diff * albedoTex * spotLight.color * spotLight.diffuseIntensity;
	vec3 specular = spec * specularTex * spotLight.color * spotLight.specularIntensity;

	float distance = length(spotLight.position - fragPos);
	float attenuation = 1.0 / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * distance * distance);

	ambient *= attenuation * intensity;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	vec3 result = (ambient + diffuse + specular);
	return result;
}

void main()
{
//	vec3 normal = normalize(normal);
//	vec3 viewDir = normalize(viewPos - fragPos);
//	vec3 albedoTex = texture(texture_diffuse1, texCoord).rgb;
//	vec3 specularTex = texture(texture_specular1, texCoord).rgb;
//
//	vec3 result = vec3(0);
//	for (int i = 0; i < directionalLightCount; i++)
//	{
//		result += calculateDirLightContribution(directionalLights[i], albedoTex, specularTex, normal, viewDir);
//	}
//	for (int i = 0; i < pointLightCount; i++)
//	{
//		result += calculatePointLightContribution(pointLights[i], albedoTex, specularTex, normal, viewDir);
//	}
//	for (int i = 0; i < spotLightCount; i++)
//	{
//		result += calculateSpotLigthContribution(spotLights[i], albedoTex, specularTex, normal, viewDir);
//	}

	fragColor = vec4(outlineColor, 1.0);
}