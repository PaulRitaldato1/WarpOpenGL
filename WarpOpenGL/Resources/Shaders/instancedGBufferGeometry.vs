#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

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
    vec4 worldPos = models[gl_InstanceID] * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoords = aTexCoords;

    mat3 normalMatrix = transpose(inverse(mat3(models[gl_InstanceID])));
    Normal = normalMatrix * aNormal;

    gl_Position = projection * view * worldPos;
}