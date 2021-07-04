#include <Buffers/GLBuffers/UniformBufferObject.h>

UniformBufferObject::UniformBufferObject(uint size, uint bindingPoint) : m_size(size), m_bindingPoint(bindingPoint)
{
	glGenBuffers(1, &m_id);
	Bind();
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	BindRange(0, size);
}

void UniformBufferObject::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_id);
}

void UniformBufferObject::Unbind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBufferObject::BindRange(uint begin, uint end)
{
	glBindBufferRange(GL_UNIFORM_BUFFER, m_bindingPoint, m_id, 0, m_size);
}

