#pragma once
#include <glad/glad.h>
#include <Common/CommonTypes.h>
#include <Texture/stb_image.h>
#include <stdexcept>
#include <Texture/ITexture.h>
#include <Common/Profiler.h>

struct GLTexParams
{
	GLTexParams() = default;
	~GLTexParams()
	{
		stbi_image_free(Data);
	}

	GLTexParams(const GLTexParams& source)
	{
		Width = source.Width;
		Height = source.Height;
		Path = source.Path;
		NumComponents = source.NumComponents;
		Format = source.Format;
		Data = source.Data;
	}

	int Width;
	int Height;

	string Path;
	string Type;
	int NumComponents;
	GLenum Format;
	unsigned char* Data;
};

inline Ref<GLTexParams> loadGLTexture(const char* path, const char* directory, string type)
{
	PROFILE_FUNCTION();

	string fullPath = string(directory) + "/" + string(path);

	Ref<GLTexParams> params = std::make_shared<GLTexParams>();
	params->Path = fullPath;
	//params->shortPath = string(path);
	params->Data = stbi_load(fullPath.c_str(), &params->Width, &params->Height, &params->NumComponents, 0);
	params->Type = type;
	if (params->Data)
	{
		if (params->NumComponents == 1)
			params->Format = GL_RED;
		else if (params->NumComponents == 3)
			params->Format = GL_RGB;
		else if (params->NumComponents == 4)
			params->Format = GL_RGBA;
	}
	else
	{
		stbi_image_free(params->Data);
		throw new std::runtime_error("Failed to load texture at path: " + fullPath);
	}

	return params;
}

struct GLTexture : ITexture
{

	GLTexture(Ref<GLTexParams> params)
	{
		PROFILE_FUNCTION();

		Width = params->Width;
		Height = params->Height;
		Path = params->Path;
		Type = params->Type;
		Format = params->Format;
		TexType = GL_TEXTURE_2D;

		glGenTextures(1, &Id);

		glBindTexture(GL_TEXTURE_2D, Id);
		glTexImage2D(GL_TEXTURE_2D, 0, params->Format, params->Width, params->Height, 0, params->Format, GL_UNSIGNED_BYTE, params->Data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	GLTexture(GLenum internalFormat, GLenum format, GLenum texType, GLenum dataType, uint width, uint height)
	{
		Format = format;
		Width = width;
		Height = height;
		Type = "texture_buffer";
		TexType = texType;
		glGenTextures(1, &Id);
		glBindTexture(texType, Id);
		glTexImage2D(texType, 0, internalFormat, width, height, 0, format, dataType, NULL);
		glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	GLTexture(const GLTexture& source)
	{
		Id = source.Id;
		Width = source.Width;
		Height = source.Height;
		Path = source.Path;
		Type = source.Type;
	}

	~GLTexture()
	{
		glDeleteTextures(1, &Id);
	}

	void setParameteri(GLenum paramName, int param)
	{
		glTexParameteri(TexType, paramName, param);
	}

	void setParameterfv(GLenum paramName, const float* data)
	{
		glTexParameterfv(TexType, paramName, data);
	}

	void Bind(uint slot = 0) const
	{
		PROFILE_FUNCTION();

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, Id);
	}

	void Unbind() const
	{
		PROFILE_FUNCTION();

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLenum Format;
	GLenum TexType;
};