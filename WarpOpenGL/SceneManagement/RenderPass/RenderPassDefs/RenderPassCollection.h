#pragma once
#include <Common/Assert.h>
#include <Common/CommonTypes.h>
#include <Common/GPUMarker.h>
#include <Common/Profiler.h>
#include <Mesh/Model.h>
#include <Buffers/GLBuffers/GLFramebuffer.h>
#include <SceneManagement/Scene/Scene.h>
#include <SceneManagement/RenderPass/RenderPassManager.h>
#include <Shader/ShaderManager.h>

struct GBuffer : public IPassData
{

	GBuffer()
	{
		bIsValid = true;
	}

	Ref<GLTexture> PositionBuffer; //color buffer
	Ref<GLTexture> NormalBuffer; //color buffer
	Ref<GLTexture> DiffuseSpec; //color buffer
	Ref<GLRenderBuffer> RBO; //for depth

	Ref<GLFramebuffer> FrameBuffer;

	uint InstancedGeometryShaderId;
	uint GeometryShaderId;
};

class RenderPassCollection
{
public:
	RenderPassCollection() : RenderPassShaderManager("Resources/Shaders/ShaderDefs/ShaderDefs.json")
	{
	}

	void AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& models);

	void AddDebugDepthPass(uint width, uint height);

	void AddOpaquePass(Scene& scene);

	void AddShadowPass();

	void AddPostProcessingPass();

	void AddGBufferPass(Scene& scene);

	void AddGBufferLightingPass(Scene& scene);

	void AddDebugQuadDraw(Vector<Ref<GLTexture>>& textures);

	ShaderManager& GetShaderManager() { return RenderPassShaderManager; }
private:
	ShaderManager RenderPassShaderManager;
};