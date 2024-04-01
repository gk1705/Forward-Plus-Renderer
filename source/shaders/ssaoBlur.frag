#version 460 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D ssaoTexture;

void main() {
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float result = 0.0;
    for (int x = -2; x < 2; ++x)
    {
        for (int y = -2; y < 2; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(ssaoTexture, texCoords + offset).r;
        }
    }
    fragColor = vec4(vec3(result / (4.0 * 4.0)), 1.0);
}