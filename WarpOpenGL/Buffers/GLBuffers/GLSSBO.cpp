#include <Buffers/GLBuffers/GLSSBO.h>

GLSSBO::GLSSBO(uint size, uint bindingPoint) : Size(size), BindingPoint(bindingPoint), CurrentOffset(0)
{
	glGenBuffers(1, &m_id);
	Bind();
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STATIC_DRAW);
	BindRange(0, size);
	bIsDirty = true;
}

void GLSSBO::Bind() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
}

void GLSSBO::Unbind() const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GLSSBO::BindRange(uint begin, uint end)
{
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, BindingPoint, m_id, 0, Size);
	bIsDirty = true;
}

