#version 460 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;

const float offset = 1.0 / 300.0;

vec3 kernelOperation()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset),
        vec2( 0.0f,    offset),
        vec2( offset,  offset),
        vec2(-offset,  0.0f),
        vec2( 0.0f,    0.0f),
        vec2( offset,  0.0f),
        vec2(-offset, -offset),
        vec2( 0.0f,   -offset),
        vec2( offset, -offset)
    );

    float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    return col;
}

void main()
{
    vec3 texColor = texture(screenTexture, texCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, texCoords).rgb;

    texColor += bloomColor;

    texColor = texColor / (texColor + vec3(1.0));
    texColor = pow(texColor, vec3(1.0 / 2.2));

    fragColor = vec4(texColor, 1.0);

    //vec3 result = pow(texColor, vec3(1.0 / gamma));
    //float depthValue = texture(screenTexture, texCoords).r;
    //fragColor = vec4(vec3(depthValue), 1.0);

    //fragColor = vec4(vec3(1.0 - texColor), 1.0);

    //float average = (texColor.r + texColor.g + texColor.b) / 3.0;
    //fragColor = vec4(average, average, average, 1.0);

//	fragColor = vec4(kernelOperation(), 1.0);
}