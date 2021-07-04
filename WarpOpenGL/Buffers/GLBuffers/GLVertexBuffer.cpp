#include <Buffers/GLBuffers/GLVertexBuffer.h>
#include <glad/glad.h>

GLVertexBuffer::GLVertexBuffer(const void* data, uint size)
{
	PROFILE_FUNCTION();

	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ARRAY_BUFFER, m_id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

GLVertexBuffer::~GLVertexBuffer()
{

	glDeleteBuffers(1, &m_id);
}

void GLVertexBuffer::Bind() const 
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, m_id);
}

void GLVertexBuffer::Unbind() const
{
	PROFILE_FUNCTION();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
