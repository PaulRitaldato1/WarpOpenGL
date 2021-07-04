#include <Buffers/GLBuffers/GLIndexBuffer.h>
#include <glad/glad.h>

GLIndexBuffer::GLIndexBuffer(const uint* data, uint count)
{
	PROFILE_FUNCTION();

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
}

GLIndexBuffer::~GLIndexBuffer()
{
	PROFILE_FUNCTION();

	glDeleteBuffers(1, &m_id);
}

void GLIndexBuffer::Bind() const
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void GLIndexBuffer::Unbind() const
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
