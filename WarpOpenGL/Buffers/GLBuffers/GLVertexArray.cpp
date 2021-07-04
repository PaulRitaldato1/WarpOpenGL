#include <Buffers/GLBuffers/GLVertexArray.h>
#include <glad/glad.h>
#include <Common/CommonTypes.h>

GLVertexArray::GLVertexArray()
{
	PROFILE_FUNCTION();

	glGenVertexArrays(1, &m_id);
}

GLVertexArray::~GLVertexArray()
{
	PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &m_id);
}

void GLVertexArray::AddBuffer(const GLVertexBuffer& vb, const GLVertexBufferLayout& layout)
{
	PROFILE_FUNCTION();

	vb.Bind();
	const auto& elements = layout.getElements();
	for (uint i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)element.offset);
	}
}

void GLVertexArray::Bind() const
{
	PROFILE_FUNCTION();

	glBindVertexArray(m_id);
}

void GLVertexArray::Unbind() const
{
	PROFILE_FUNCTION();

	glBindVertexArray(0);
}
