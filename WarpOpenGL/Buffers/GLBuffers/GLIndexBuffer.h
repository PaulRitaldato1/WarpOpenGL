#pragma once

#include <Buffers/GenericBuffer.h>
class GLIndexBuffer : public GenericBuffer
{
private:
	uint Count;
public:
	GLIndexBuffer(const uint* data, uint count);
	~GLIndexBuffer();

	void Bind() const override;
	void Unbind() const override;

	uint getCount() { return Count; }
};