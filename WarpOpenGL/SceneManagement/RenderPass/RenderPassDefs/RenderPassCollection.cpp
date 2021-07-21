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

			g_renderer.ClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
			
			gbuffer->geometryShader = m_shaderManager.getShaderId("gBufferGeometry");
			gbuffer->instancedGeometryShader = m_shaderManager.getShaderId("InstancedGBufferGeometry");


			//need a good way to get window current size instead of hard coding 
			gbuffer->positionBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->positionBuffer->setParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			gbuffer->positionBuffer->setParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			gbuffer->normalBuffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			gbuffer->normalBuffer->setParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			gbuffer->normalBuffer->setParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			gbuffer->diffuseSpec = std::make_shared<GLTexture>(GL_RGBA, GL_RGBA, GL_TEXTURE_2D, GL_UNSIGNED_BYTE, 1920, 1080);
			gbuffer->diffuseSpec->setParameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			gbuffer->diffuseSpec->setParameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			gbuffer->rbo = std::make_shared<GLRenderBuffer>(1920, 1080, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);

			gbuffer->frameBuffer = std::make_shared<GLFramebuffer>();
			gbuffer->frameBuffer->Bind();
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->positionBuffer);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->diffuseSpec);
			gbuffer->frameBuffer->AttachTexture(AttachmentType::COLOR, gbuffer->normalBuffer);
			gbuffer->frameBuffer->setDrawBuffers();
			gbuffer->frameBuffer->AttachRenderBuffer(gbuffer->rbo);
			gbuffer->frameBuffer->Unbind();
			FATAL_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Frambuffer machine broke");

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
					model->Draw(m_shaderManager.getShaderById(data->geometryShader));
				}
				else
				{
					model->Draw(m_shaderManager.getShaderById(data->instancedGeometryShader));
				}
			}

			data->frameBuffer->Unbind();

			ImGui::Begin("Tex Debug");
			ImGui::Image((void*)(intptr_t)data->normalBuffer->Id, ImVec2(512, 512));
			ImGui::End();
			return data;
		});
}

void RenderPassCollection::AddGBufferLightingPass(Scene& scene)
{
	struct GBufferLightingData : IPassData
	{
		GBufferLightingData()
		{
			isValid = true;
		}
		
		uint gBufferLightingShaderId;
		Ref<GLTexture> buffer; //color buffer
		Ref<GLTexture> buffer2;
		Ref<GLRenderBuffer> rbo; //for depth
		Ref<GLFramebuffer> frameBuffer;

	};
	
	g_renderPassManager.AddPass("GBufferLightingPass", "GBufferPass", 
		[&](Ref<IPassData> passData) 
		{
			Ref<GBufferLightingData> lightData = std::make_shared<GBufferLightingData>();

			lightData->gBufferLightingShaderId = m_shaderManager.getShaderId("gBufferLighting");
			lightData->rbo = std::make_shared<GLRenderBuffer>(1920, 1080, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
			lightData->buffer = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);
			lightData->buffer2 = std::make_shared<GLTexture>(GL_RGBA16F, GL_RGBA, GL_TEXTURE_2D, GL_FLOAT, 1920, 1080);

			lightData->frameBuffer = std::make_shared<GLFramebuffer>();
			lightData->frameBuffer->Bind();
			lightData->frameBuffer->AttachTexture(AttachmentType::COLOR, lightData->buffer);
			lightData->frameBuffer->AttachTexture(AttachmentType::COLOR, lightData->buffer2);
			lightData->frameBuffer->setDrawBuffers();
			lightData->frameBuffer->AttachRenderBuffer(lightData->rbo);
			lightData->frameBuffer->Unbind();

			return lightData;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<GBuffer> gbuffer = std::dynamic_pointer_cast<GBuffer>(dependencyData);
			Ref<GBufferLightingData> lightingData = std::dynamic_pointer_cast<GBufferLightingData>(setupData);

			//lightingData->frameBuffer->Bind();

			auto& shader = m_shaderManager.getShaderById(lightingData->gBufferLightingShaderId);
			g_renderer.ClearBoundBufferBits();
			shader.Bind();

			g_renderer.setCullMode(GL_FRONT);
			for (const auto& pointlight : scene.getPointlights())
			{
				auto shaderParams = pointlight.getShaderParams();

				shader.setUniform("gPos", 0);
				gbuffer->positionBuffer->Bind(0);
				shader.setUniform("gDiffuseSpec", 1);
				gbuffer->diffuseSpec->Bind(1);
				shader.setUniform("gNormal", 2);
				gbuffer->normalBuffer->Bind(2);

				shader.setUniform("viewPos", scene.getActiveCamera().getPosition());

				shader.setUniform("light.position", shaderParams.position);
				shader.setUniform("light.direction", shaderParams.direction);
				shader.setUniform("light.color", shaderParams.color);
				shader.setUniform("light.spotlightAngle", shaderParams.spotlightAngle);
				shader.setUniform("light.radius", shaderParams.radius);
				shader.setUniform("light.intensity", shaderParams.intensity);
				shader.setUniform("light.enable", shaderParams.enable);
				shader.setUniform("light.type", (uint)shaderParams.type);
				shader.setUniform("light.linear", (float)0.7);
				shader.setUniform("light.linear", (float)1.8);

				/*g_renderer.DrawIndexed(pointlight.getLightVolume().getIndexedDrawCall());*/
				pointlight.getLightVolume().Draw(shader);

			}

			g_renderer.setCullMode(GL_BACK);

			//lightingData->frameBuffer->Unbind();

			return gbuffer;
		});
}

void RenderPassCollection::AddDebugQuadDraw(Vector<Ref<GLTexture>>& textures)
{
	struct QuadDrawData : IPassData
	{
		QuadDrawData()
		{
			isValid = true;
		}

		uint shaderId;
		Ref<GLVertexBuffer> vbo;
		Ref<GLVertexArray> vao;
		Array<float, 20> vertices;
	};

	g_renderPassManager.AddPass("QuadPass",
		[&](Ref<IPassData> passData) 
		{
			Ref<QuadDrawData> data = std::make_shared<QuadDrawData>();
			data->vertices[0] = -1.0f;
			data->vertices[1] = 1.0f;
			data->vertices[2] = 0.0f;
			data->vertices[3] = 0.0f;
			data->vertices[4] = 1.0f;

			data->vertices[5] = -1.0f;
			data->vertices[6] = -1.0f;
			data->vertices[7] = 0.0f;
			data->vertices[8] = 0.0f;
			data->vertices[9] = 0.0f;

			data->vertices[10] = 1.0f;
			data->vertices[11] = 1.0f;
			data->vertices[12] = 0.0f;
			data->vertices[13] = 1.0f;
			data->vertices[14] = 1.0f;

			data->vertices[15] = 1.0f;
			data->vertices[16] = -1.0f;
			data->vertices[17] = 0.0f;
			data->vertices[18] = 1.0f;
			data->vertices[19] = 0.0f;
			
			data->vbo = std::make_shared<GLVertexBuffer>(data->vertices.data(), sizeof(float) * data->vertices.size());
			data->vao = std::make_shared<GLVertexArray>();

			GLVertexBufferLayout layout;
			layout.Push<float>(3);
			layout.Push<float>(2);
			data->vao->AddBuffer(*data->vbo, layout);

			data->shaderId = m_shaderManager.getShaderId("QuadDraw");

			return data;
		}, 
		[&](Ref<IPassData> dependencyData, Ref<IPassData> setupData)
		{
			Ref<QuadDrawData> data = std::dynamic_pointer_cast<QuadDrawData>(setupData);

			auto& shader = m_shaderManager.getShaderById(data->shaderId);
			shader.Bind();
			
			shader.setUniform("tex", 0);
			textures[0]->Bind(0);

			shader.setUniform("tex1", 1);
			textures[1]->Bind(1);

			shader.setUniform("tex2", 2);
			textures[2]->Bind(2);

			g_renderer.DrawArrays(GL_TRIANGLE_STRIP, 0, 4, *data->vao);
			
			return data;
		});
}