#pragma once
#include <Common/Assert.h>
#include <Common/CommonTypes.h>
#include <Common/GPUMarker.h>
#include <Common/Profiler.h>
#include <Mesh/Model.h>
#include <Buffers/GLBuffers/GLFramebuffer.h>

#include <SceneManagement/RenderPass/RenderPassManager.h>

struct ZOnlyPassData : public IPassData
{
	ZOnlyPassData()
	{
		isValid = true;
	}

	Ref<GLTexture> depthTexture;
	Ref<GLFramebuffer> frameBuffer;
	Array<Ref<Shader>, 2> shaders;
};

void AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& models);

void AddDebugDepthPass();

void AddOpaquePass(Vector<Ref<Model>>& modelList);

void AddShadowPass();

void AddPostProcessingPass();