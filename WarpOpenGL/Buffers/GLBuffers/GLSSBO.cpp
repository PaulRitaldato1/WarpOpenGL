#include <Buffers/GLBuffers/GLSSBO.h>

GLSSBO::GLSSBO(uint size, uint bindingPoint) : m_size(size), m_bindingPoint(bindingPoint), m_currentOffset(0)
{
	glGenBuffers(1, &m_id);
	Bind();
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_STATIC_DRAW);
	BindRange(0, size);
	m_isDirty = true;
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
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, m_bindingPoint, m_id, 0, m_size);
	m_isDirty = true;
}

