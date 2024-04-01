#version 460 core

in vec4 fragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
    float lightDistance = length(fragPos.xyz - lightPos);
    lightDistance = lightDistance / farPlane; // map to [0;1] range

    gl_FragDepth = lightDistance;
}