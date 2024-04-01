#version 330 core

out vec4 fragColor;

in vec3 normal;
in vec3 position;

uniform vec3 viewPos;
uniform samplerCube skybox;

void main()
{
    vec3 I = normalize(position - viewPos);
    vec3 R = reflect(I, normalize(normal));
    fragColor = vec4(texture(skybox, R).rgb, 1.0);
}