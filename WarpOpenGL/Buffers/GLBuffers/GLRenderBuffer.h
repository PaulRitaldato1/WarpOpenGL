#pragma once
#include <Buffers/GenericBuffer.h>
#include <glad/glad.h>

class GLRenderBuffer : public GenericBuffer
{
public:

	GLRenderBuffer(uint width, uint height, GLenum format);
	~GLRenderBuffer();

	void Bind() const override;


	void Unbind() const override;

	GLenum getFormat() { return m_format; }

private:
	GLenum m_format;
};