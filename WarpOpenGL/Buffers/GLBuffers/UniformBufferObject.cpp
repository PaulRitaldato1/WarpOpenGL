#include <Buffers/GLBuffers/UniformBufferObject.h>

UniformBufferObject::UniformBufferObject(uint size, uint bindingPoint) : Size(size), BindingPoint(bindingPoint)
{
	glGenBuffers(1, &Id);
	Bind();
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	BindRange(0, size);
}

void UniformBufferObject::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, Id);
}

void UniformBufferObject::Unbind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBufferObject::BindRange(uint begin, uint end)
{
	glBindBufferRange(GL_UNIFORM_BUFFER, BindingPoint, Id, 0, Size);
}

