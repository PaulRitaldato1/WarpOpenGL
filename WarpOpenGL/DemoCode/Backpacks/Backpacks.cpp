#include "Backpacks.h"

#include <Includes/glm/glm.hpp>

#include <Lighting/Pointlight.h>
#include <Mesh/Model.h>
#include <Mesh/ModelLoader.h>

Ref<Scene> BackPacksDemo::MakeScene(GLFWwindow* window)
{
	ModelLoader Loader("Resources/");
	FPCamera Camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0, 1.0, 0.0), -90.0, 0);

	std::vector<glm::vec3> ObjectPositions;
	ObjectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	ObjectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	ObjectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	ObjectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	ObjectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	ObjectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	ObjectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	ObjectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	ObjectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

	Vector<glm::mat4> ModelMatrices;
	glm::mat4 Base = glm::mat4(1.0f);
	Base = glm::scale(Base, glm::vec3(1.0f, 1.0f, 1.0f));

	for (auto& Pos : ObjectPositions)
		ModelMatrices.push_back(glm::translate(Base, Pos));
	
	Vector<ModelDesc> ModelDescs;
	ModelDescs.emplace_back("Resources/Backpack/backpack.obj", false, true, ModelMatrices);

	Vector<Ref<Model>> Models = Loader.LoadModelsAsync(ModelDescs, 1);

	Models[0]->SetInstances(ModelMatrices);

	Vector<Pointlight> PointLights;
	Vector<Spotlight> Spotlights;
	Vector<DirectionalLight> DirectionalLights;
	DirectionalLights.emplace_back(glm::vec3(1.0, 1.0, 1.0), glm::vec3(-0.5, -1.0, 0.0), .75, false);

	Vector<FPCamera> Cameras;
	Cameras.push_back(Camera);

	DemoScene = std::make_shared<Scene>(window, "Backpacks Test", Spotlights, PointLights, DirectionalLights, Models, Cameras);
	return DemoScene;
}

void BackPacksDemo::Update(float DeltaTime)
{
	float BaseRotAngle = 0.5f;
	float AdjustedRotAngle = BaseRotAngle / (DeltaTime * 10000);

	for (auto& ModelItem : DemoScene->GetModels())
	{
		auto Instances = ModelItem->GetInstances();

		for (auto& InstanceItem : Instances)
		{
			InstanceItem = glm::rotate(InstanceItem, AdjustedRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		ModelItem->SetInstances(Instances);
	}
}
