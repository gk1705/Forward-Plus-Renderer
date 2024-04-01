#version 460 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D image;

uniform bool horizontal;
uniform float weights[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
     vec2 texOffset = 1.0 / textureSize(image, 0);
     vec3 result = texture(image, texCoords).rgb * weights[0];
     if(horizontal)
     {
         for(int i = 1; i < 5; ++i)
         {
            result += texture(image, texCoords + vec2(texOffset.x * i, 0.0)).rgb * weights[i];
            result += texture(image, texCoords - vec2(texOffset.x * i, 0.0)).rgb * weights[i];
         }
     }
     else
     {
         for(int i = 1; i < 5; ++i)
         {
             result += texture(image, texCoords + vec2(0.0, texOffset.y * i)).rgb * weights[i];
             result += texture(image, texCoords - vec2(0.0, texOffset.y * i)).rgb * weights[i];
         }
     }
     fragColor = vec4(result, 1.0);
}