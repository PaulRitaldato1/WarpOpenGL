#pragma once
#include <Buffers/GenericBuffer.h>
#include <glad/glad.h>

class GLRenderBuffer : public GenericBuffer
{
public:

	GLRenderBuffer(uint width, uint height, GLenum componentFormat, GLenum format);
	~GLRenderBuffer();

	void Bind() const override;


	void Unbind() const override;

	GLenum getFormat() { return Format; }

private:
	GLenum Format;
};