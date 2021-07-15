#include <SceneManagement/RenderPass/RenderPassDefs/RenderPassCollection.h>
#include <Buffers/GLBuffers/GLSSBO.h>
#include <Buffers/GLBuffers/UniformBufferObject.h>
#include <Texture/GLTexture/GLTexture.h>
#include <Renderer/GLRenderer/GLRenderer.h>
#include <time.h>
#include <Shader/ShaderManager.h>

RenderPassManager g_renderPassManager;


void RenderPassCollection::AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& models)
{
	struct ZOnlyPassData : public IPassData
	{
		ZOnlyPassData()
		{
			isValid = true;
		}

		Ref<GLTexture> depthTexture;
		Ref<GLFramebuffer> frameBuffer;
		uint shaderId;
		uint instancedShaderId;
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
			zPassData->shaderId = m_shaderManager.getShaderId("ZOnly");
			zPassData->instancedShaderId = m_shaderManager.getShaderId("ZOnlyInstanced");

			return zPassData;
		},
		[=](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
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
						model->Draw(m_shaderManager.getShaderById(data->shaderId));
					}
					else
						model->Draw(m_shaderManager.getShaderById(data->instancedShaderId));
				}
			}

			data->frameBuffer->Unbind();
			g_renderer.setCullMode(GL_BACK);
			g_renderer.setViewport(1920, 1080);
			return data;
		});
}

void RenderPassCollection::RenderPassCollection::AddOpaquePass(Vector<Ref<Model>>& modelList)
{
	struct OpaquePassData : public IPassData
	{
		OpaquePassData()
		{
			isValid = true;
		}

		//Vector<glm::mat4> instancedModelMatrices;
		uint shaderId;
		uint instancedShaderId;
		Vector<Ref<Model>> models;
	};

	g_renderPassManager.AddPass("OpaquePass",
		[&](Ref<IPassData> passData) 
		{
			Ref<OpaquePassData> data = std::make_shared<OpaquePassData>();

			data->shaderId = m_shaderManager.getShaderId("BasicModel");
			data->instancedShaderId = m_shaderManager.getShaderId("InstancedBasicModel");

			data->models = modelList;

			return data;
		},
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<OpaquePassData> data = std::dynamic_pointer_cast<OpaquePassData>(setupData);

			g_renderer.ClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			g_renderer.ClearBoundBufferBits();

			for (auto& model : data->models)
			{
				if (!model->getIsInstanced())
				{
					model->Draw(m_shaderManager.getShaderById(data->shaderId));
				}
				else
				{
					model->Draw(m_shaderManager.getShaderById(data->instancedShaderId));
				}
			}

			return setupData;
		});
}

void RenderPassCollection::AddShadowPass()
{

}

void RenderPassCollection::AddPostProcessingPass()
{

}

void RenderPassCollection::AddGBufferPass(Scene& scene)
{

	g_renderPassManager.AddPass("GBufferPass",
		[&](Ref<IPassData> data) 
		{
			
			Ref<GBuffer> gbuffer = std::make_shared<GBuffer>();
			
			//gbuffer->geometryShader = std::make_shared<Shader>("Resources/Shaders/gBufferGeometry.vs", "Resources/Shaders/gBufferGeometry.fs");
			//gbuffer->instancedGeometryShader = std::make_shared<Shader>("Resources/Shaders/instancedGBufferGeometry.vs", "Resources/Shaders/gBufferGeometry.fs");


			//need a good way to get window current size instead of hardcoding 
			gbuffer->lightAccumulationBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->diffuseBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->normalBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->specBuffer = std::make_shared<GLTexture>(GL_RGBA, GL_RGBA, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, 1920, 1080);
			gbuffer->rbo = std::make_shared<GLRenderBuffer>(1920, 1080, GL_DEPTH_ATTACHMENT);

			gbuffer->frameBuffer = std::make_shared<GLFramebuffer>();
			
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->lightAccumulationBuffer);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->diffuseBuffer);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->normalBuffer);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->specBuffer);
			gbuffer->frameBuffer->AttachRenderBuffer(gbuffer->rbo);

			return gbuffer;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData) 
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

void RenderPassCollection::AddGBufferLightingPass(Scene& scene)
{
	struct GBufferLighting : IPassData
	{
		GBufferLighting()
		{
			isValid = true;
		}

		Ref<Shader> gBufferLightingShader;
	};
	
	g_renderPassManager.AddPass("GBufferLightingPass", "GBufferPass", 
		[&](Ref<IPassData> passData) 
		{
			Ref<GBufferLighting> lightData = std::make_shared<GBufferLighting>();

			//lightData->gBufferLightingShader = std::make_shared<Shader>("Resources/Shaders/gBufferLighting.vs", "Resources/Shaders/gBufferLighting.fs");

			return lightData;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<GBuffer> gbuffer = std::dynamic_pointer_cast<GBuffer>(dependencyData);
			Ref<GBufferLighting> lightingData = std::dynamic_pointer_cast<GBufferLighting>(setupData);

			return gbuffer;
		});
}
