#include <SceneManagement/RenderPass/RenderPassDefs/RenderPassCollection.h>
#include <Buffers/GLBuffers/GLSSBO.h>
#include <Buffers/GLBuffers/UniformBufferObject.h>
#include <Texture/GLTexture/GLTexture.h>
#include <Renderer/GLRenderer/GLRenderer.h>
#include <time.h>
#include <Shader/ShaderManager.h>
#include <Imgui/ImGuiWrapper.h>
#include <Buffers/GLBuffers/GLVertexArray.h>
#include <Buffers/GLBuffers/GLVertexBuffer.h>

RenderPassManager g_renderPassManager;


void RenderPassCollection::AddZOnlyPass(uint width, uint height, Vector<Ref<Model>>& Models)
{
	struct ZOnlyPassData : public IPassData
	{
		ZOnlyPassData()
		{
			bIsValid = true;
		}

		Ref<GLTexture> DepthTexture;
		Ref<GLFramebuffer> FrameBuffer;
		uint ShaderId;
		uint InstancedShaderId;
	};

	g_renderPassManager.AddPass("ZOnlyPass",
		[&](Ref<IPassData> passData) 
		{
			Ref<ZOnlyPassData> zPassData = std::make_shared<ZOnlyPassData>();
			zPassData->FrameBuffer = std::make_shared<GLFramebuffer>();

			zPassData->DepthTexture = std::make_shared<GLTexture>(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_TEXTURE_2D, GL_FLOAT, width, height);
			zPassData->DepthTexture->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			zPassData->DepthTexture->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			zPassData->DepthTexture->SetParameteri(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			zPassData->DepthTexture->SetParameteri(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			zPassData->DepthTexture->SetParameterfv(GL_TEXTURE_BORDER_COLOR, borderColor);

			zPassData->FrameBuffer->Bind();
			zPassData->FrameBuffer->AttachTexture(AttachmentType::DEPTH, zPassData->DepthTexture);
			g_renderer.SetReadBuffer(GL_NONE);
			g_renderer.SetDrawBuffer(GL_NONE);
			
			DYNAMIC_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer machine broke");

			zPassData->FrameBuffer->Unbind();
			zPassData->ShaderId = RenderPassShaderManager.GetShaderId("ZOnly");
			zPassData->InstancedShaderId = RenderPassShaderManager.GetShaderId("ZOnlyInstanced");

			return zPassData;
		},
		[=](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<ZOnlyPassData> data = std::dynamic_pointer_cast<ZOnlyPassData>(setupData);


			//render to depth map
			data->FrameBuffer->Bind();

			g_renderer.SetViewport(data->DepthTexture->Width, data->DepthTexture->Height);
			
			g_renderer.SetCullMode(GL_FRONT);

			g_renderer.ClearBufferBits(GL_DEPTH_BUFFER_BIT);

			for (auto& ModelItem : Models)
			{
				if (ModelItem->GetHasShadow())
				{
					if (!ModelItem->GetIsInstanced())
					{
						ModelItem->Draw(RenderPassShaderManager.GetShaderById(data->ShaderId));
					}
					else
					{
						ModelItem->Draw(RenderPassShaderManager.GetShaderById(data->InstancedShaderId));
					}
				}
			}

			data->FrameBuffer->Unbind();
			g_renderer.SetCullMode(GL_BACK);
			g_renderer.SetViewport(1920, 1080);
			return data;
		});
}

void RenderPassCollection::RenderPassCollection::AddOpaquePass(Scene& scene)
{
	struct OpaquePassData : public IPassData
	{
		OpaquePassData()
		{
			bIsValid = true;
		}

		//Vector<glm::mat4> instancedModelMatrices;
		uint ShaderId;
		uint InstancedShaderId;
		Vector<Ref<Model>> Models;
	};

	g_renderPassManager.AddPass("OpaquePass",
		[&](Ref<IPassData> passData) 
		{
			Ref<OpaquePassData> Data = std::make_shared<OpaquePassData>();

			Data->ShaderId = RenderPassShaderManager.GetShaderId("BasicModel");
			Data->InstancedShaderId = RenderPassShaderManager.GetShaderId("InstancedBasicModel");

			Data->Models = scene.GetModels();

			return Data;
		},
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<OpaquePassData> Data = std::dynamic_pointer_cast<OpaquePassData>(setupData);

			g_renderer.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			g_renderer.ClearBoundBufferBits();

			for (auto& ModelItem : Data->Models)
			{
				if (!ModelItem->GetIsInstanced())
				{
					ModelItem->Draw(RenderPassShaderManager.GetShaderById(Data->ShaderId));
				}
				else
				{
					ModelItem->Draw(RenderPassShaderManager.GetShaderById(Data->InstancedShaderId));
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

void RenderPassCollection::AddGBufferPass(Scene& Scene)
{

	g_renderPassManager.AddPass("GBufferPass",
		[&](Ref<IPassData> data) 
		{
			
			Ref<GBuffer> GBufferData = std::make_shared<GBuffer>();
			
			GBufferData->GeometryShaderId = RenderPassShaderManager.GetShaderId("gBufferGeometry");
			GBufferData->InstancedGeometryShaderId = RenderPassShaderManager.GetShaderId("InstancedGBufferGeometry");

			Tuple<int, int> WindowSize = Scene.GetWindowSize();
			int width = std::get<0>(WindowSize);
			int height = std::get<1>(WindowSize);

			//need a good way to get window current size instead of hard coding 
			GBufferData->PositionBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, width, height);
			GBufferData->PositionBuffer->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			GBufferData->PositionBuffer->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			GBufferData->NormalBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, width, height);
			GBufferData->NormalBuffer->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			GBufferData->NormalBuffer->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			GBufferData->DiffuseSpec = std::make_shared<GLTexture>(GL_RGBA, GL_RGBA, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, width, height);
			GBufferData->DiffuseSpec->SetParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			GBufferData->DiffuseSpec->SetParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			GBufferData->RBO = std::make_shared<GLRenderBuffer>(width, height, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);

			GBufferData->FrameBuffer = std::make_shared<GLFramebuffer>();
			GBufferData->FrameBuffer->Bind();
			GBufferData->FrameBuffer->AttachTexture(AttachmentType::COLOR, GBufferData->PositionBuffer);
			GBufferData->FrameBuffer->AttachTexture(AttachmentType::COLOR, GBufferData->DiffuseSpec);
			GBufferData->FrameBuffer->AttachTexture(AttachmentType::COLOR, GBufferData->NormalBuffer);
			GBufferData->FrameBuffer->SetDrawBuffers();
			GBufferData->FrameBuffer->AttachRenderBuffer(GBufferData->RBO);
			GBufferData->FrameBuffer->Unbind();
			FATAL_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer machine broke");
			
			//auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			//if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
			//{
			//	std::cout << "Framebuffer not complete " << fboStatus << std::endl;
			//}

			return GBufferData;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData) 
		{
			Ref<GBuffer> GBufferData = std::dynamic_pointer_cast<GBuffer>(setupData);

			GBufferData->FrameBuffer->Bind();

			g_renderer.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			g_renderer.ClearBoundBufferBits();

			
			for (auto ModelItem : Scene.GetModels())
			{
				if (!ModelItem->GetIsInstanced())
				{
					ModelItem->Draw(RenderPassShaderManager.GetShaderById(GBufferData->GeometryShaderId));
				}
				else
				{
					ModelItem->Draw(RenderPassShaderManager.GetShaderById(GBufferData->InstancedGeometryShaderId));
				}
			}

			GBufferData->FrameBuffer->Unbind();

			return GBufferData;
		});
}

void RenderPassCollection::AddGBufferLightingPass(Scene& scene)
{
	struct GBufferLightingData : IPassData
	{
		GBufferLightingData()
		{
			bIsValid = true;
		}
		
		uint gBufferLightingShaderId;
		uint gBufferQuadLightingShaderId;
	};
	
	g_renderPassManager.AddPass("GBufferLightingPass", "GBufferPass", 
		[&](Ref<IPassData> passData) 
		{
			Ref<GBufferLightingData> LightData = std::make_shared<GBufferLightingData>();

			LightData->gBufferLightingShaderId = RenderPassShaderManager.GetShaderId("gBufferLighting");
			LightData->gBufferQuadLightingShaderId = RenderPassShaderManager.GetShaderId("QuadGBufferLighting");

			/*lightData->rbo = std::make_shared<GLRenderBuffer>(1920, 1080, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
			lightData->buffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			lightData->buffer2 = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);

			lightData->frameBuffer = std::make_shared<GLFramebuffer>();
			lightData->frameBuffer->Bind();
			lightData->frameBuffer->AttachTexture(AttachmentType::COLOR, lightData->buffer);
			lightData->frameBuffer->AttachTexture(AttachmentType::COLOR, lightData->buffer2);
			lightData->frameBuffer->setDrawBuffers();
			lightData->frameBuffer->AttachRenderBuffer(lightData->rbo);
			lightData->frameBuffer->Unbind();*/

			return LightData;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<GBuffer> gBuffer = std::dynamic_pointer_cast<GBuffer>(dependencyData);
			Ref<GBufferLightingData> lightingData = std::dynamic_pointer_cast<GBufferLightingData>(setupData);

			auto& LightVolumeShader = RenderPassShaderManager.GetShaderById(lightingData->gBufferLightingShaderId);
			auto& QuadShader = RenderPassShaderManager.GetShaderById(lightingData->gBufferQuadLightingShaderId);
			g_renderer.ClearBoundBufferBits();
			g_renderer.Enable(GL_BLEND);
			g_renderer.SetBlendFunc(GL_ONE, GL_ONE);
			g_renderer.SetBlendEquation(GL_FUNC_ADD);

			g_renderer.Disable(GL_DEPTH_TEST);
			
			g_renderer.SetCullMode(GL_FRONT);
			LightVolumeShader.Bind();

			LightVolumeShader.SetUniform("gPos", 0);
			gBuffer->PositionBuffer->Bind(0);
			LightVolumeShader.SetUniform("gDiffuseSpec", 1);
			gBuffer->DiffuseSpec->Bind(1);
			LightVolumeShader.SetUniform("gNormal", 2);
			gBuffer->NormalBuffer->Bind(2);


			for (const auto& PointLight : scene.GetPointlights())
			{
				auto ShaderParams = PointLight.GetShaderParams();


				LightVolumeShader.SetUniform("viewPos", scene.GetActiveCamera().GetPosition());

				LightVolumeShader.SetUniform("light.position", ShaderParams.Position);
				LightVolumeShader.SetUniform("light.direction", ShaderParams.Direction);
				LightVolumeShader.SetUniform("light.color", ShaderParams.Color);
				LightVolumeShader.SetUniform("light.spotlightAngle", ShaderParams.SpotlightAngle);
				LightVolumeShader.SetUniform("light.radius", ShaderParams.Radius);
				LightVolumeShader.SetUniform("light.intensity", ShaderParams.Intensity);
				LightVolumeShader.SetUniform("light.enable", ShaderParams.bEnable);
				LightVolumeShader.SetUniform("light.type", (uint)ShaderParams.Type);
				LightVolumeShader.SetUniform("light.linear", (float)0.7);
				LightVolumeShader.SetUniform("light.quadratic", (float)1.8);

				PointLight.GetLightVolume().Draw(LightVolumeShader);
			}
			g_renderer.SetCullMode(GL_BACK);

			QuadShader.Bind();
			QuadShader.SetUniform("gPos", 0);
			QuadShader.SetUniform("gDiffuseSpec", 1);
			QuadShader.SetUniform("gNormal", 2);

			gBuffer->PositionBuffer->Bind(0);
			gBuffer->DiffuseSpec->Bind(1);
			gBuffer->NormalBuffer->Bind(2);



			for (const auto& DirectionalLight : scene.GetDirectionalLights())
			{
				auto ShaderParams = DirectionalLight.GetShaderParams();


				LightVolumeShader.SetUniform("viewPos", scene.GetActiveCamera().GetPosition());

				LightVolumeShader.SetUniform("light.position", ShaderParams.Position);
				LightVolumeShader.SetUniform("light.direction", ShaderParams.Direction);
				LightVolumeShader.SetUniform("light.color", ShaderParams.Color);
				LightVolumeShader.SetUniform("light.spotlightAngle", ShaderParams.SpotlightAngle);
				LightVolumeShader.SetUniform("light.radius", ShaderParams.Radius);
				LightVolumeShader.SetUniform("light.intensity", ShaderParams.Intensity);
				LightVolumeShader.SetUniform("light.enable", ShaderParams.bEnable);
				LightVolumeShader.SetUniform("light.type", (uint)ShaderParams.Type);
				LightVolumeShader.SetUniform("light.linear", (float)0.7);
				LightVolumeShader.SetUniform("light.quadratic", (float)1.8);

				DirectionalLight.GetLightVolume().Draw(QuadShader);
			}

			g_renderer.Disable(GL_BLEND);
			g_renderer.Enable(GL_DEPTH_TEST);

			return gBuffer;
		});
}

void RenderPassCollection::AddDebugQuadDraw(Vector<Ref<GLTexture>>& Textures)
{
	struct QuadDrawData : IPassData
	{
		QuadDrawData()
		{
			bIsValid = true;
		}

		uint ShaderId;
		Ref<WarpMesh> Mesh;
	};

	g_renderPassManager.AddPass("QuadPass",
		[&](Ref<IPassData> passData) 
		{
			Ref<QuadDrawData> QuadData = std::make_shared<QuadDrawData>();
			QuadData->Mesh = GeoGen::CreateDefaultQuad();
			QuadData->ShaderId = RenderPassShaderManager.GetShaderId("QuadDraw");

			return QuadData;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<QuadDrawData> QuadData = std::dynamic_pointer_cast<QuadDrawData>(setupData);

			auto& Shader = RenderPassShaderManager.GetShaderById(QuadData->ShaderId);
			Shader.Bind();
			
			int i = 0;
			for (auto& textureItem : Textures)
			{
				Shader.SetUniform("tex" + std::to_string(i), i);
				textureItem->Bind(++i);
			}

			QuadData->Mesh->Draw(Shader);
			return QuadData;
		});
}