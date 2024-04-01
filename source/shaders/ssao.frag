#version 460 core

in vec2 texCoords;

uniform sampler2D depthTexture;
uniform sampler2D noiseTexture;

#define SAMPLES_SIZE 64
uniform vec3 samples[SAMPLES_SIZE];

uniform int kernelSize;
uniform float kernelRadius;
uniform float bias;
uniform vec2 noiseScale;

uniform mat4 inverseProjectionMatrix;
uniform mat4 projectionMatrix;

out vec4 fragColor;

vec3 calculateViewPosition(vec2 texCoords)
{
	float fragDepth = texture(depthTexture, texCoords).r;
	vec4 ndc = vec4(texCoords * 2.0 - 1.0, 2.0 * fragDepth - 1.0, 1.0);
	vec4 clip = inverseProjectionMatrix * ndc;
	vec3 viewPos = clip.xyz / clip.w;

	return viewPos;
}

void main()
{
	vec3 viewPos = calculateViewPosition(texCoords);
	vec3 normal = cross(dFdy(viewPos.xyz), dFdx(viewPos.xyz));
	normal = normalize(normal * -1.0);
	vec3 random = texture(noiseTexture, texCoords * noiseScale).xyz;
	vec3 T = normalize(random - normal * dot(random, normal));
	vec3 B = cross(normal, T);
	mat3 TBN = mat3(T, B, normal);

	float occlusion = 0.0;
	for (int i = 0; i < SAMPLES_SIZE; i++)
	{
		vec3 samplePos = TBN * samples[i];
		samplePos = viewPos + samplePos * kernelRadius;

		vec4 offset = projectionMatrix * vec4(samplePos, 1.0);
		offset.xy /= offset.w;
		offset.xy = offset.xy * 0.5 + 0.5;

		float sampleDepth = calculateViewPosition(offset.xy).z;
		float rangeCheck = smoothstep(0.0, 1.0, kernelRadius / abs(viewPos.z - sampleDepth));

		//occlusion += (sampleDepth >= samplePos.z + 0.0001 ? 1.0 : 0.0) * rangeCheck;
		occlusion += float(sampleDepth >= samplePos.z + bias) * rangeCheck;
	}

	float avgOcclusion = occlusion / float(SAMPLES_SIZE);
	avgOcclusion = 1.0 - avgOcclusion;
	avgOcclusion = pow(avgOcclusion, 2.0);
	fragColor = vec4(vec3(avgOcclusion), 1.0);
}