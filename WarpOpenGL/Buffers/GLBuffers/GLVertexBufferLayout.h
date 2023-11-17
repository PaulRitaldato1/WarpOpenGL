#pragma once
#include <Common/CommonTypes.h>
#include <Common/Assert.h>
#include <glad/glad.h>
#include <Mesh/Vertex.h>

struct GLVertexBufferElement
{
	uint Type;
	uint Count;
	unsigned char Normalized;
	uint Offset;
	static uint GetSizeOfType(uint type)
	{
		switch (type)
		{
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
		}
		FATAL_ASSERT(false, "Unsupported type was sent");
		return 0;
	}
};

class GLVertexBufferLayout
{
private:
	Vector<GLVertexBufferElement> Elements;
	uint Stride;
public:

	GLVertexBufferLayout()
		: Stride(0) {}

	template<typename T>
	void Push(uint count)
	{
		FATAL_ASSERT(false, "This template should not have been generated");
	}

	template<>
	void Push<float>(uint count)
	{
		Elements.push_back({ GL_FLOAT, count, GL_FALSE, Stride });
		Stride += count * GLVertexBufferElement::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<uint>(uint count)
	{
		Elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE, Stride });
		Stride += count * GLVertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(uint count)
	{
		Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE, Stride });
		Stride += count * GLVertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	template<>
	void Push<Vertex>(uint count)
	{
		for (uint i = 0; i < count; i++)
		{
			//could probably use reflection here to loop over the 
			//elements of the struct itself instead of manual/
			//but setting up reflection is a bitch
			Push<float>(3);
			Push<float>(3);
			Push<float>(2);
			Push<float>(3);
			Push<float>(3);
		}
	}

	const Vector<GLVertexBufferElement>& GetElements() const { return Elements; }
	uint GetStride() const { return Stride; }
};