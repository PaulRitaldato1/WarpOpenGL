#include <Renderer/GLRenderer/GLRenderer.h>

GLRenderer g_renderer;

void GLRenderer::ClearBufferBits(uint buffersToClear)
{
	glClear(buffersToClear);
}

void GLRenderer::ClearBoundBufferBits()
{
	glClear(m_boundBufferbits);
}

void GLRenderer::EnableBufferBits(uint bufferToEnable)
{
	glEnable(bufferToEnable);
	m_boundBufferbits |= bufferToEnable;
}

void GLRenderer::Enable(uint toEnable)
{
	glEnable(toEnable);
}

void GLRenderer::Disable(uint toDisable)
{
	glDisable(toDisable);
}

void GLRenderer::setBlendFunc(uint sFactor, uint blendFunc)
{
	glBlendFunc(sFactor, blendFunc);
}

void GLRenderer::setBlendEquation(GLenum mode)
{
	glBlendEquation(mode);
}

void GLRenderer::ClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void GLRenderer::ClearColor(glm::vec4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void GLRenderer::setViewport(uint width, uint height)
{
	glViewport(0, 0, width, height);
}

void GLRenderer::setCullMode(GLenum mode)
{
	glCullFace(mode);
}

void GLRenderer::setDrawBuffer(GLenum setting)
{
	glDrawBuffer(setting);
}

void GLRenderer::setReadBuffer(GLenum setting)
{
	glReadBuffer(setting);
}

void GLRenderer::DrawIndexed(const GLIndexedDrawCall& args)
{
	args.vertexArrayBuffer->Bind();
	args.indexBuffer->Bind();
	glDrawElements(GL_TRIANGLES, args.primitiveCount, GL_UNSIGNED_INT, 0);
}

void GLRenderer::DrawInstanced(GLInstancedDrawCall& args)
{
	args.vertexArrayBuffer->Bind();
	args.indexBuffer->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, args.indexCount, GL_UNSIGNED_INT, 0, args.instanceCount);
}

void GLRenderer::DrawArrays(uint topology, uint first, uint count, GLVertexArray& vao)
{
	vao.Bind();
	glDrawArrays(topology, first, count);
}

void GLRenderer::setDrawWireFrame(GLenum mode)
{
	glPolygonMode(GL_FRONT_AND_BACK, mode);
}
