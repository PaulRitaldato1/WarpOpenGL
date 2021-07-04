#pragma once

#include <Buffers/GenericBuffer.h>
class GLIndexBuffer : public GenericBuffer
{
private:
	uint m_count;
public:
	GLIndexBuffer(const uint* data, uint count);
	~GLIndexBuffer();

	void Bind() const override;
	void Unbind() const override;

	uint getCount() { return m_count; }
};