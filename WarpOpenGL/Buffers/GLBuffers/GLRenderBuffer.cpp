#include <Buffers/GLBuffers/GLRenderBuffer.h>

GLRenderBuffer::GLRenderBuffer(uint width, uint height, GLenum componentFormat, GLenum format) : Format(format)
{
	glGenRenderbuffers(1, &Id);
	Bind();
	glRenderbufferStorage(GL_RENDERBUFFER, componentFormat, width, height);
	Unbind();
}

GLRenderBuffer::~GLRenderBuffer()
{
	glDeleteRenderbuffers(1, &Id);
}

void GLRenderBuffer::Bind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, Id);
}

void GLRenderBuffer::Unbind() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

