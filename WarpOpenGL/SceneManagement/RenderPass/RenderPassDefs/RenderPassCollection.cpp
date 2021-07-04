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

			zPassData->frameBuffer->AttachTexture(GL_DEPTH_ATTACHMENT, zPassData->depthTexture);

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
			
			g_renderer.setReadBuffer(GL_NONE);
			g_renderer.setDrawBuffer(GL_NONE);

			g_renderer.ClearBufferBits(GL_DEPTH_BUFFER_BIT);

			for (auto& model : models)
			{
				if (!model->getIsInstanced())
				{
					model->Draw(*data->shaders[0]);
				}
				else
					model->Draw(*data->shaders[1]);
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

			//uint amount = 100000;
			//Vector<glm::mat4> modelMatrices;

			//srand(time(NULL)); // initialize random seed	
			//float radius = 150.0;
			//float offset = 25.0f;
			//for (unsigned int i = 0; i < amount; i++)
			//{
			//	glm::mat4 model = glm::mat4(1.0f);
			//	// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			//	float angle = (float)i / (float)amount * 360.0f;
			//	float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			//	float x = sin(angle) * radius + displacement;
			//	displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			//	float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			//	displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
			//	float z = cos(angle) * radius + displacement;
			//	model = glm::translate(model, glm::vec3(x, y, z));

			//	// 2. scale: Scale between 0.05 and 0.25f
			//	float scale = (rand() % 20) / 100.0f + 0.05;
			//	model = glm::scale(model, glm::vec3(scale));

			//	// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			//	float rotAngle = (rand() % 360);
			//	model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			//	// 4. now add to list of matrices
			//	data->instancedModelMatrices.push_back(model);

			//}

			data->models = modelList;
			//data->modelMatrix = glm::mat4(1.0f);
			//data->modelMatrix = glm::translate(data->modelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
			//data->modelMatrix = glm::scale(data->modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));

			return data;
		},
		[&](Ref<IPassData> passData, Ref<IPassData> setupData)
		{
			Ref<OpaquePassData> data = std::dynamic_pointer_cast<OpaquePassData>(setupData);

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