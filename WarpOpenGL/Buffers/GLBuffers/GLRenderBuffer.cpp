#include <Buffers/GLBuffers/GLRenderBuffer.h>

GLRenderBuffer::GLRenderBuffer(uint width, uint height, GLenum format) : m_format(format)
{
	glGenRenderbuffers(1, &m_id);
	glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
}

GLRenderBuffer::~GLRenderBuffer()
{
	glDeleteRenderbuffers(1, &m_id);
}

void GLRenderBuffer::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_id);
}

void GLRenderBuffer::Unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

