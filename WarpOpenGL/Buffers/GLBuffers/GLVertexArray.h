#pragma once
#include <Buffers/GLBuffers/GLVertexBufferLayout.h>
#include <Buffers/GLBuffers/GLVertexBuffer.h>

class GLVertexArray : public GenericBuffer
{
public:
	GLVertexArray();
	~GLVertexArray();

	void AddBuffer(const GLVertexBuffer& vb, const GLVertexBufferLayout& layout);
	void Bind() const override;
	void Unbind() const override;
};