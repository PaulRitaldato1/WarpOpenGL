#pragma once
#include <Buffers/GenericBuffer.h>

class GLVertexBuffer : public GenericBuffer
{
public:
	GLVertexBuffer(const void* data, uint size);
	~GLVertexBuffer();
	void Bind() const  override;
	void Unbind() const override;
};