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
	uint primitiveCount = 0;

	Ref<GLVertexArray> vertexArrayBuffer;
	Ref<GLIndexBuffer> indexBuffer;
};

struct GLInstancedDrawCall : public IDrawCall
{
	uint instanceCount = 0;
	uint indexCount = 0;
	Ref<GLVertexArray> vertexArrayBuffer;
	Ref<GLIndexBuffer> indexBuffer;
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

	void setBlendFunc(uint sFactor, uint blendFunc);
	void setBlendEquation(GLenum mode);

	void ClearColor(float r, float g, float b, float a);
	void ClearColor(glm::vec4 color);

	void setViewport(uint width, uint height);

	void setCullMode(GLenum mode);

	void setDrawBuffer(GLenum setting);
	void setReadBuffer(GLenum setting);

	void DrawIndexed(const GLIndexedDrawCall& args);
	void DrawInstanced(GLInstancedDrawCall& args);
	void DrawArrays(uint topology, uint first, uint count, GLVertexArray& vao);

	void setDrawWireFrame(GLenum mode);
private:
	uint m_boundBufferbits = 0;
};

extern GLRenderer g_renderer;