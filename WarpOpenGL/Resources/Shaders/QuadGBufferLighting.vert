#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal; //unused but the mesh will default bind these locations
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = vec4(aPos, 1.0);
}