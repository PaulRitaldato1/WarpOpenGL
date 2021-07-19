#version 460 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D tex;
uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;
uniform sampler2D tex4;

void main()
{
	FragColor = texture(tex, TexCoords) * texture(tex1, TexCoords) * texture(tex2, TexCoords);
}