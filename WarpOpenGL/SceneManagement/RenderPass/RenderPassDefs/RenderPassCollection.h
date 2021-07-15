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
		isValid = true;
	}

	Ref<GLTexture> lightAccumulationBuffer; //color buffer
	Ref<GLTexture> diffuseBuffer;
	Ref<GLTexture> normalBuffer; //color buffer
	Ref<GLTexture> specBuffer; //color buffer
	Ref<GLRenderBuffer> rbo; //for depth

	Ref<GLFramebuffer> frameBuffer;

	Ref<Shader> instancedGeometryShader;
	Ref<Shader> geometryShader;
};

class RenderPassCollection
{
public:
	RenderPassCollection() : m_shaderManager("Resources/Shaders/ShaderDefs/ShaderDefs.json")
	{
	}

	void AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& models);

	void AddDebugDepthPass(uint width, uint height);

	void AddOpaquePass(Vector<Ref<Model>>& modelList);

	void AddShadowPass();

	void AddPostProcessingPass();

	void AddGBufferPass(Scene& scene);

	void AddGBufferLightingPass(Scene& scene);
private:
	ShaderManager m_shaderManager;
};