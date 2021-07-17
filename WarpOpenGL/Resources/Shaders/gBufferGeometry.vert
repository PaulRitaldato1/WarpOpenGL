#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out vec2 TexCoords;
out vec3 NormalVS;
out vec3 TangentVS;
out vec3 BinormalVS;

layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

void main()
{
    TexCoords = aTexCoords;

    //convert to view space
    //openGL is column major order, for HLSL switch to model * view 
    NormalVS = mat3(view * model) * aNormal;
    TangentVS = mat3(view * model) * aTangent;
    BinormalVS = mat3(view * model) * aBitangent;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}