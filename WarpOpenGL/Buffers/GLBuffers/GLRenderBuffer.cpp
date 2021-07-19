#include <Buffers/GLBuffers/GLRenderBuffer.h>

GLRenderBuffer::GLRenderBuffer(uint width, uint height, GLenum componentFormat, GLenum format) : m_format(format)
{
	glGenRenderbuffers(1, &m_id);
	Bind();
	glRenderbufferStorage(GL_RENDERBUFFER, componentFormat, width, height);
	Unbind();
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

