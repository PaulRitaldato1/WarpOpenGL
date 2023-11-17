#pragma once
#include <Renderer/IRenderer.h>
#include <glad/glad.h>
#include <Common/CommonTypes.h>
#include <glm/glm.hpp>
#include <Buffers/GLBuffers/GLIndexBuffer.h>
#include <Buffers/GLBuffers/GLVertexArray.h>

/*Draw call defs*/
//could probably make these just one struct, but I like that this is more clear

struct IDrawCall
{
	//empty for now
};

struct GLIndexedDrawCall : public IDrawCall
{
	uint PrimitiveCount = 0;

	Ref<GLVertexArray> VertexArrayBuffer;
	Ref<GLIndexBuffer> IndexBuffer;
};

struct GLInstancedDrawCall : public IDrawCall
{
	uint instanceCount = 0;
	uint IndexCount = 0;
	Ref<GLVertexArray> VertexArrayBuffer;
	Ref<GLIndexBuffer> IndexBuffer;
};

///////////////////////////////////////////////////////////

class GLRenderer : public IRenderer
{
public:

	void ClearBufferBits(uint buffersToClear);
	void ClearBoundBufferBits();
	void EnableBufferBits(uint bufferToEnable);

	void Enable(uint toEnable);
	void Disable(uint toDisable);

	void SetBlendFunc(uint sFactor, uint blendFunc);
	void SetBlendEquation(GLenum mode);

	void ClearColor(float r, float g, float b, float a);
	void ClearColor(glm::vec4 color);

	void SetViewport(uint width, uint height);

	void SetCullMode(GLenum mode);

	void SetDrawBuffer(GLenum setting);
	void SetReadBuffer(GLenum setting);

	void DrawIndexed(const GLIndexedDrawCall& args);
	void DrawInstanced(GLInstancedDrawCall& args);
	void DrawArrays(uint topology, uint first, uint count, GLVertexArray& vao);

	void SetDrawWireFrame(GLenum mode);
private:
	uint BoundBufferbits = 0;
};

extern GLRenderer g_renderer;