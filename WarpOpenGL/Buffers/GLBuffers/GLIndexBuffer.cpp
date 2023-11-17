#include <Buffers/GLBuffers/GLIndexBuffer.h>
#include <glad/glad.h>

GLIndexBuffer::GLIndexBuffer(const uint* data, uint count)
{
	PROFILE_FUNCTION();

	glGenBuffers(1, &Id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
}

GLIndexBuffer::~GLIndexBuffer()
{
	PROFILE_FUNCTION();

	glDeleteBuffers(1, &Id);
}

void GLIndexBuffer::Bind() const
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Id);
}

void GLIndexBuffer::Unbind() const
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
