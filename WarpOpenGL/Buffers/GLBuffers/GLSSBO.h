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
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, CurrentOffset, sizeof(T), &data);
		UpdateOffset(sizeof(T));
		Unbind();
	}


	template<>
	void BindSubdata<Vector<glm::mat4>>(const Vector<glm::mat4>& data)
	{
		Bind();
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, CurrentOffset, data.size() * sizeof(glm::mat4), data.data());
		UpdateOffset(data.size() * sizeof(glm::mat4));
		Unbind();
		bIsDirty = false;
	}
	void BindRange(uint begin, uint end);

	bool IsDirty()
	{
		return bIsDirty;
	}

private:

	void UpdateOffset(uint size)
	{
		FATAL_ASSERT(CurrentOffset + size <= Size, "Attemping to bind more memory than buffer contains!");

		if (CurrentOffset + size == Size)
		{
			CurrentOffset = 0;
		}
		else
		{
			CurrentOffset += size;
		}
	}

	uint CurrentOffset;
	uint Size;
	uint BindingPoint;
	bool bIsDirty;
};
