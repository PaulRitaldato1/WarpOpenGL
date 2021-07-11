#include <SceneManagement/RenderPass/RenderPassDefs/RenderPassCollection.h>
#include <Buffers/GLBuffers/GLSSBO.h>
#include <Buffers/GLBuffers/UniformBufferObject.h>
#include <Texture/GLTexture/GLTexture.h>
#include <Renderer/GLRenderer/GLRenderer.h>
#include <Shader/Shader.h>
#include <time.h>

RenderPassManager g_renderPassManager;


void AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& models)
{
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

	g_renderPassManager.AddPass("ZOnlyPass",
		[&](Ref<IPassData> passData) 
		{
			Ref<ZOnlyPassData> zPassData = std::make_shared<ZOnlyPassData>();
			zPassData->frameBuffer = std::make_shared<GLFramebuffer>();

			zPassData->depthTexture = std::make_shared<GLTexture>(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_TEXTURE_2D, GL_FLOAT, width, height);
			zPassData->depthTexture->setParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			zPassData->depthTexture->setParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			zPassData->depthTexture->setParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			zPassData->depthTexture->setParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			zPassData->depthTexture->setParameterfv(GL_TEXTURE_BORDER_COLOR, borderColor);

			zPassData->frameBuffer->Bind();
			zPassData->frameBuffer->AttachTexture(AttachmentType::DEPTH, zPassData->depthTexture);
			g_renderer.setReadBuffer(GL_NONE);
			g_renderer.setDrawBuffer(GL_NONE);
			
			DYNAMIC_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer machine broke");

			zPassData->frameBuffer->Unbind();
			zPassData->shaders[0] = std::make_shared<Shader>("Resources/Shaders/ZOnly.vs", "Resources/Shaders/ZOnly.fs");
			zPassData->shaders[1] = std::make_shared<Shader>("Resources/Shaders/ZOnlyInstanced.vs", "Resources/Shaders/ZOnly.fs");

			return zPassData;
		},
		[=](Ref<IPassData> passData, Ref<IPassData> setupData)
		{
			Ref<ZOnlyPassData> data = std::dynamic_pointer_cast<ZOnlyPassData>(setupData);


			//render to depth map
			data->frameBuffer->Bind();

			g_renderer.setViewport(data->depthTexture->Width, data->depthTexture->Height);
			
			g_renderer.setCullMode(GL_FRONT);

			g_renderer.ClearBufferBits(GL_DEPTH_BUFFER_BIT);

			for (auto& model : models)
			{
				if (model->getHasShadow())
				{
					if (!model->getIsInstanced())
					{
						model->Draw(*data->shaders[0]);
					}
					else
						model->Draw(*data->shaders[1]);
				}
			}

			data->frameBuffer->Unbind();
			g_renderer.setCullMode(GL_BACK);
			g_renderer.setViewport(1920, 1080);
			return data;
		});
}

void AddOpaquePass(Vector<Ref<Model>>& modelList) 
{
	struct OpaquePassData : public IPassData
	{
		OpaquePassData()
		{
			isValid = true;
		}

		//Vector<glm::mat4> instancedModelMatrices;
		Array<Ref<Shader>, 2> shaders;
		Vector<Ref<Model>> models;
	};

	g_renderPassManager.AddPass("OpaquePass",
		[&](Ref<IPassData> passData) 
		{
			Ref<OpaquePassData> data = std::make_shared<OpaquePassData>();

			data->shaders[0] = std::make_shared<Shader>("Resources/Shaders/model.vs", "Resources/Shaders/model.fs");
			data->shaders[1] = std::make_shared<Shader>("Resources/Shaders/instancedModel.vs", "Resources/Shaders/model.fs");

			data->models = modelList;

			return data;
		},
		[&](Ref<IPassData> passData, Ref<IPassData> setupData)
		{
			Ref<OpaquePassData> data = std::dynamic_pointer_cast<OpaquePassData>(setupData);

			g_renderer.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			g_renderer.ClearBoundBufferBits();

			for (auto& model : data->models)
			{
				if (!model->getIsInstanced())
				{
					model->Draw(*data->shaders[0]);
				}
				else
				{
					model->Draw(*data->shaders[1]);
				}
			}

			return setupData;
		});
}

void AddShadowPass()
{

}

void AddPostProcessingPass()
{

}

void AddGBufferPass(Scene& scene)
{
	struct GBuffer : public IPassData
	{

		GBuffer()
		{
			isValid = true;
		}

		Ref<GLTexture> positionBuffer; //color buffer
		Ref<GLTexture> normalBuffer; //color buffer
		Ref<GLTexture> specBuffer; //color buffer
		Ref<GLRenderBuffer> rbo; //for depth

		Ref<GLFramebuffer> frameBuffer;

		Ref<Shader> instancedGeometryShader;
		Ref<Shader> geometryShader;
	};

	g_renderPassManager.AddPass("GBufferPass",
		[&](Ref<IPassData> data) 
		{
			
			Ref<GBuffer> gbuffer = std::make_shared<GBuffer>();
			
			gbuffer->geometryShader = std::make_shared<Shader>("Resources/Shaders/gBufferGeometry.vs", "Resources/Shaders/gBufferGeometry.fs");
			gbuffer->instancedGeometryShader = std::make_shared<Shader>("Resources/Shaders/instancedGBufferGeometry.vs", "Resources/Shaders/gBufferGeometry.fs");


			//need a good way to get window current size instead of hardcoding 
			gbuffer->positionBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->normalBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->specBuffer = std::make_shared<GLTexture>(GL_RGBA, GL_RGBA, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, 1920, 1080);
			gbuffer->rbo = std::make_shared<GLRenderBuffer>(1920, 1080, GL_DEPTH_ATTACHMENT);

			gbuffer->frameBuffer = std::make_shared<GLFramebuffer>();
			
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->positionBuffer);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->normalBuffer);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->specBuffer);
			gbuffer->frameBuffer->AttachRenderBuffer(gbuffer->rbo);

			return gbuffer;
		}, 
		[&](Ref<IPassData> passData, Ref<IPassData> setupData) 
		{
			Ref<GBuffer> data = std::dynamic_pointer_cast<GBuffer>(setupData);

			data->frameBuffer->Bind();

			g_renderer.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			g_renderer.ClearBoundBufferBits();
			
			for (auto model : scene.getModels())
			{
				if (!model->getIsInstanced())
				{
					model->Draw(*data->geometryShader);
				}
				else
				{
					model->Draw(*data->instancedGeometryShader);
				}
			}

			data->frameBuffer->Unbind();

			return data;
		});
}
