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
		glBufferSubData(GL_UNIFORM_BUFFER, CurrentOffset, sizeof(T), &data);
		UpdateOffset<T>();
		Unbind();
	}
	
	template<>
	void BindSubdata<glm::mat4>(const glm::mat4& data)
	{
		Bind();
		glBufferSubData(GL_UNIFORM_BUFFER, CurrentOffset, sizeof(glm::mat4), glm::value_ptr(data));
		UpdateOffset<glm::mat4>();
		Unbind();
	}

	//uint getId() const { return m_id; }
private:

	template <typename T>
	void UpdateOffset()
	{
		uint size = sizeof(T);

		FATAL_ASSERT(CurrentOffset + size <= Size, "Attemping to bind more memory than buffer contains!");

		if (CurrentOffset + size == Size)
			CurrentOffset = 0;
		else
			CurrentOffset += size;
	}

	void BindRange(uint begin, uint end);

	uint Size = 0;
	uint BindingPoint;
	uint CurrentOffset = 0;
};