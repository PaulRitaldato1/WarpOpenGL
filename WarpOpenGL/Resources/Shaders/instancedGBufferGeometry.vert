#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;


out vec2 TexCoords;
out vec3 Normal;
out vec3 Tangent;
out vec3 Binormal;
out vec3 FragPos;

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
    TexCoords = aTexCoords;

    vec4 worldPos = models[gl_InstanceID] * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;

    mat3 invTranspose = transpose(inverse(mat3(models[gl_InstanceID])));
    Normal = invTranspose * aNormal;
    Tangent = invTranspose * aTangent;
    Binormal = invTranspose * aBitangent;
    
    gl_Position = projection * view * models[gl_InstanceID] * vec4(aPos, 1.0);
}
