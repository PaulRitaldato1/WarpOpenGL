#include <Buffers/GLBuffers/GLFramebuffer.h>

GLFramebuffer::GLFramebuffer() : m_numColorAttachments(0)
{
	glGenFramebuffers(1, &m_id);
}


GLFramebuffer::~GLFramebuffer()
{
	glDeleteFramebuffers(1, &m_id);
}

void GLFramebuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_id);
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
		m_colorAttachments.push_back(attachmentType + m_colorAttachments.size());
	}

	uint attachmentNum = (attachmentType == AttachmentType::COLOR) ? m_colorAttachments.size() - 1 : 0;

	if (texture->TexType == GL_TEXTURE_1D)
	{
		glFramebufferTexture1D(GL_FRAMEBUFFER, attachmentType + attachmentNum, texture->TexType, texture->Id, 0);
	}
	else if (texture->TexType == GL_TEXTURE_2D)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType + attachmentNum, texture->TexType, texture->Id, 0);
	}
	//else if (texture->Format == GL_TEXTURE_3D)
	//{
	//	glFramebufferTexture3D(GL_FRAMEBUFFER, attachmentType + attachmentNum, texture->Format, texture->Id, 0);
	//}
}

void GLFramebuffer::setDrawBuffers()
{
	glDrawBuffers(m_colorAttachments.size(), m_colorAttachments.data());
}

void GLFramebuffer::AttachRenderBuffer(Ref<GLRenderBuffer> rbo)
{
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, rbo->getFormat(), GL_RENDERBUFFER, rbo->getId());
}
