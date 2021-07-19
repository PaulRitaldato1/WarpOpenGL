#pragma once
#include <Buffers/GenericBuffer.h>
#include <Buffers/GLBuffers/GLRenderBuffer.h>
#include <Texture/GLTexture/GLTexture.h>
#include <glad/glad.h>

enum AttachmentType
{
	COLOR = GL_COLOR_ATTACHMENT0,
	DEPTH = GL_DEPTH_ATTACHMENT,
	STENCIL = GL_STENCIL_ATTACHMENT,
	DEPTH_STENCIL = GL_DEPTH_STENCIL_ATTACHMENT
};

class GLFramebuffer : public GenericBuffer
{

public:

	GLFramebuffer();
	~GLFramebuffer();
	void Bind() const override;

	void Unbind() const override;

	void AttachTexture(GLenum attachmentType, Ref<GLTexture> texture);
	
	void setDrawBuffers();
	void AttachRenderBuffer(Ref<GLRenderBuffer> rbo);

private:
	uint m_numColorAttachments;
	Vector<uint> m_colorAttachments;
};