#version 460 core

out vec4 fragColor;

in vec3 localPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);

vec2 sampleSphericalMap(vec3 viewDir)
{
	vec2 uv = vec2(atan(viewDir.z, viewDir.x), asin(viewDir.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec3 viewDir = normalize(localPos);
	vec2 uv = sampleSphericalMap(viewDir);
	vec3 color = texture(equirectangularMap, uv).rgb;
	fragColor = vec4(color, 1.0);
}