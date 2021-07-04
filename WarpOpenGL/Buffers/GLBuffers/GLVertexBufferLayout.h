#pragma once
#include <Common/CommonTypes.h>
#include <Common/Assert.h>
#include <glad/glad.h>
#include <Mesh/Vertex.h>

struct GLVertexBufferElement
{
	uint type;
	uint count;
	unsigned char normalized;
	uint offset;
	static uint getSizeOfType(uint type)
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
	Vector<GLVertexBufferElement> m_elements;
	uint m_stride;
public:

	GLVertexBufferLayout()
		: m_stride(0) {}

	template<typename T>
	void Push(uint count)
	{
		FATAL_ASSERT(false, "This template should not have been generated");
	}

	template<>
	void Push<float>(uint count)
	{
		m_elements.push_back({ GL_FLOAT, count, GL_FALSE, m_stride });
		m_stride += count * GLVertexBufferElement::getSizeOfType(GL_FLOAT);
	}

	template<>
	void Push<uint>(uint count)
	{
		m_elements.push_back({GL_UNSIGNED_INT, count, GL_FALSE, m_stride });
		m_stride += count * GLVertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void Push<unsigned char>(uint count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE, m_stride });
		m_stride += count * GLVertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
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

	const Vector<GLVertexBufferElement>& getElements() const { return m_elements; }
	uint getStride() const { return m_stride; }
};