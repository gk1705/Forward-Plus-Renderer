#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec2 texCoord;
out vec3 normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 normalMatrix;

uniform float outline;

void main()
{
	fragPos = vec3(modelMatrix * vec4(aPos, 1.0));
	texCoord = aTexCoord;
	normal = mat3(normalMatrix) * aNormal;
	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPos + aNormal * outline, 1.0);
}