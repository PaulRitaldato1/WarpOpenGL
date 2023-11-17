#include <Buffers/GLBuffers/GLVertexBuffer.h>
#include <glad/glad.h>

GLVertexBuffer::GLVertexBuffer(const void* data, uint size)
{
	PROFILE_FUNCTION();

	glGenBuffers(1, &Id);
	glBindBuffer(GL_ARRAY_BUFFER, Id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

GLVertexBuffer::~GLVertexBuffer()
{

	glDeleteBuffers(1, &Id);
}

void GLVertexBuffer::Bind() const 
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, Id);
}

void GLVertexBuffer::Unbind() const
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
