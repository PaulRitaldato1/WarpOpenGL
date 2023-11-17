#include <Buffers/GLBuffers/GLVertexArray.h>
#include <glad/glad.h>
#include <Common/CommonTypes.h>

GLVertexArray::GLVertexArray()
{
	PROFILE_FUNCTION();

	glGenVertexArrays(1, &Id);
}

GLVertexArray::~GLVertexArray()
{
	PROFILE_FUNCTION();

	glDeleteVertexArrays(1, &Id);
}

void GLVertexArray::AddBuffer(const GLVertexBuffer& vb, const GLVertexBufferLayout& layout)
{
	PROFILE_FUNCTION();

	vb.Bind();
	const auto& Elements = layout.GetElements();
	for (uint i = 0; i < Elements.size(); i++)
	{
		const auto& Element = Elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, Element.Count, Element.Type, Element.Normalized, layout.GetStride(), (const void*)Element.Offset);
	}
}

void GLVertexArray::Bind() const
{
	PROFILE_FUNCTION();

	glBindVertexArray(Id);
}

void GLVertexArray::Unbind() const
{
	PROFILE_FUNCTION();

	glBindVertexArray(0);
}
