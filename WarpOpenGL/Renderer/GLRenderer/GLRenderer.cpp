#include <Renderer/GLRenderer/GLRenderer.h>

GLRenderer g_renderer;

void GLRenderer::ClearBufferBits(uint buffersToClear)
{
	glClear(buffersToClear);
}

void GLRenderer::ClearBoundBufferBits()
{
	glClear(BoundBufferbits);
}

void GLRenderer::EnableBufferBits(uint bufferToEnable)
{
	glEnable(bufferToEnable);
	BoundBufferbits |= bufferToEnable;
}

void GLRenderer::Enable(uint toEnable)
{
	glEnable(toEnable);
}

void GLRenderer::Disable(uint toDisable)
{
	glDisable(toDisable);
}

void GLRenderer::SetBlendFunc(uint sFactor, uint blendFunc)
{
	glBlendFunc(sFactor, blendFunc);
}

void GLRenderer::SetBlendEquation(GLenum mode)
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

void GLRenderer::SetViewport(uint width, uint height)
{
	glViewport(0, 0, width, height);
}

void GLRenderer::SetCullMode(GLenum mode)
{
	glCullFace(mode);
}

void GLRenderer::SetDrawBuffer(GLenum setting)
{
	glDrawBuffer(setting);
}

void GLRenderer::SetReadBuffer(GLenum setting)
{
	glReadBuffer(setting);
}

void GLRenderer::DrawIndexed(const GLIndexedDrawCall& args)
{
	args.VertexArrayBuffer->Bind();
	args.IndexBuffer->Bind();
	glDrawElements(GL_TRIANGLES, args.PrimitiveCount, GL_UNSIGNED_INT, 0);
}

void GLRenderer::DrawInstanced(GLInstancedDrawCall& args)
{
	args.VertexArrayBuffer->Bind();
	args.IndexBuffer->Bind();
	glDrawElementsInstanced(GL_TRIANGLES, args.IndexCount, GL_UNSIGNED_INT, 0, args.instanceCount);
}

void GLRenderer::DrawArrays(uint topology, uint first, uint count, GLVertexArray& vao)
{
	vao.Bind();
	glDrawArrays(topology, first, count);
}

void GLRenderer::SetDrawWireFrame(GLenum mode)
{
	glPolygonMode(GL_FRONT_AND_BACK, mode);
}
