#pragma once
#include <Buffers/GenericBuffer.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Common/Assert.h>

class GLSSBO : public GenericBuffer
{
public:
	GLSSBO(uint size, uint bindingPoint);
	

	void Bind() const override;

	void Unbind() const override;

	template<typename T>
	void BindSubdata(const T& data)
	{
		Bind();
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_currentOffset, sizeof(T), &data);
		updateOffset(sizeof(T));
		Unbind();
	}


	template<>
	void BindSubdata<Vector<glm::mat4>>(const Vector<glm::mat4>& data)
	{
		Bind();
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, m_currentOffset, data.size() * sizeof(glm::mat4), data.data());
		updateOffset(data.size() * sizeof(glm::mat4));
		Unbind();
	}

private:

	void BindRange(uint begin, uint end);
	void updateOffset(uint size)
	{
		FATAL_ASSERT(m_currentOffset + size <= m_size, "Attemping to bind more memory than buffer contains!");

		if (m_currentOffset + size == m_size)
			m_currentOffset = 0;
		else
			m_currentOffset += size;
	}

	uint m_currentOffset;
	uint m_size;
	uint m_bindingPoint;
};
