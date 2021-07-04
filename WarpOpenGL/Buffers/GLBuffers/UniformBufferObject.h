#pragma once
#include <Buffers/GenericBuffer.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Common/Assert.h>

class UniformBufferObject : public GenericBuffer
{

public:

	UniformBufferObject(uint size, uint bindingPoint);
	void Bind() const override;

	void Unbind() const override;


	template<typename T>
	void BindSubdata(const T& data)
	{
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, m_currentOffset, sizeof(T), &data);
		updateOffset<T>();
		Unbind();
	}
	
	template<>
	void BindSubdata<glm::mat4>(const glm::mat4& data)
	{
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, m_currentOffset, sizeof(glm::mat4), glm::value_ptr(data));
		updateOffset<glm::mat4>();
		Unbind();
	}

	//uint getId() const { return m_id; }
private:

	template <typename T>
	void updateOffset()
	{
		uint size = sizeof(T);

		FATAL_ASSERT(m_currentOffset + size <= m_size, "Attemping to bind more memory than buffer contains!");

		if (m_currentOffset + size == m_size)
			m_currentOffset = 0;
		else
			m_currentOffset += size;
	}

	void BindRange(uint begin, uint end);

	uint m_size = 0;
	uint m_bindingPoint;
	uint m_currentOffset = 0;
};