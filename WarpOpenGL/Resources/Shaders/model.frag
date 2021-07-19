#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse0;
//uniform sampler2D texture_specular0;

void main()
{    
    FragColor = texture(texture_diffuse0, TexCoords);
//    FragColor.rgb = texture(texture_diffuse0, TexCoords).rgb;
//    FragColor.a = test.r;
}