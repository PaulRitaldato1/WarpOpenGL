#include <Buffers/GLBuffers/GLFramebuffer.h>

GLFramebuffer::GLFramebuffer() : m_numColorAttachments(0)
{
	glGenFramebuffers(1, &Id);
}


GLFramebuffer::~GLFramebuffer()
{
	glDeleteFramebuffers(1, &Id);
}

void GLFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, Id);
}

void GLFramebuffer::Unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLFramebuffer::AttachTexture(GLenum attachmentType, Ref<GLTexture> texture)
{

	//update color attachment enum vector
	if (attachmentType == AttachmentType::COLOR)
	{
		ColorAttachments.push_back(attachmentType + ColorAttachments.size());
	}

	uint AttachmentNum = (attachmentType == AttachmentType::COLOR) ? ColorAttachments.size() - 1 : 0;

	if (texture->TexType == GL_TEXTURE_1D)
	{
		glFramebufferTexture1D(GL_FRAMEBUFFER, attachmentType + AttachmentNum, texture->TexType, texture->Id, 0);
	}
	else if (texture->TexType == GL_TEXTURE_2D)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType + AttachmentNum, texture->TexType, texture->Id, 0);
	}
	//else if (texture->Format == GL_TEXTURE_3D)
	//{
	//	glFramebufferTexture3D(GL_FRAMEBUFFER, attachmentType + attachmentNum, texture->Format, texture->Id, 0);
	//}
}

void GLFramebuffer::SetDrawBuffers()
{
	glDrawBuffers(ColorAttachments.size(), ColorAttachments.data());
}

void GLFramebuffer::AttachRenderBuffer(Ref<GLRenderBuffer> rbo)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbo->getFormat(), GL_RENDERBUFFER, rbo->getId());
}
