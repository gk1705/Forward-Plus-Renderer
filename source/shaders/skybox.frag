#version 460 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
    //vec3 envColor = textureLod(skybox, texCoords, 2).rgb;
    fragColor = texture(skybox, texCoords);
    brightColor = vec4(0.0); // no bloom for skybox, for now
}