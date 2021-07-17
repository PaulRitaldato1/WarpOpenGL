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

layout (std140, binding = 1) buffer InstancedModels
{
    mat4 models[];
};

void main()
{
    NormalVS = mat3(view * models[gl_InstanceID]) * aNormal;
    TangentVS = mat3(view * models[gl_InstanceID]) * aTangent;
    BinormalVS = mat3(view * models[gl_InstanceID]) * aBitangent;
    
    gl_Position = projection * view * models[gl_InstanceID] * vec4(aPos, 1.0);
}
