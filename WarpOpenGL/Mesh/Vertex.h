#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex
{
	Vertex() = default;
	Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 texCoords, glm::vec3 tangent, glm::vec3 bitangent) : 
		Position(pos)
		, Normal(normal)
		, TexCoords(texCoords)
		, Tangent(tangent)
		, Bitangent(bitangent)
	{}

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};