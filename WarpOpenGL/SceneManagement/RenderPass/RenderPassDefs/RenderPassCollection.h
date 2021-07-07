#pragma once
#include <Common/Assert.h>
#include <Common/CommonTypes.h>
#include <Common/GPUMarker.h>
#include <Common/Profiler.h>
#include <Mesh/Model.h>
#include <Buffers/GLBuffers/GLFramebuffer.h>

#include <SceneManagement/RenderPass/RenderPassManager.h>

void AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& models);

void AddDebugDepthPass(uint width, uint height);

void AddOpaquePass(Vector<Ref<Model>>& modelList);

void AddShadowPass();

void AddPostProcessingPass();