#include "Backpacks.h"

#include <Includes/glm/glm.hpp>

#include <Lighting/Pointlight.h>
#include <Mesh/Model.h>
#include <Mesh/ModelLoader.h>

Ref<Scene> BackPacksDemo::MakeScene(GLFWwindow* window)
{
	ModelLoader loader("Resources/");
	FPCamera camera(glm::vec3(0.0f, 0.0f, 50.0f), glm::vec3(0.0, 1.0, 0.0), -90.0, 0);

	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

	Vector<glm::mat4> modelMatrices;
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::scale(base, glm::vec3(1.0f, 1.0f, 1.0f));

	for (auto& pos : objectPositions)
		modelMatrices.push_back(glm::translate(base, pos));
	
	Vector<ModelDesc> modelDescs;
	modelDescs.emplace_back("Resources/Backpack/backpack.obj", false, true, modelMatrices);

	Vector<Ref<Model>> models = loader.loadModelsAsync(modelDescs, 1);

	models[0]->setInstances(modelMatrices);

	Vector<Pointlight> pointLights;
	Vector<Spotlight> spotlights;
	Vector<DirectionalLight> directionalLights;
	directionalLights.emplace_back(glm::vec3(1.0, 1.0, 1.0), glm::vec3(-0.5, -1.0, 0.0), .75, false);

	Vector<FPCamera> cameras;
	cameras.push_back(camera);

	m_scene = std::make_shared<Scene>(window, "Backpacks Test", spotlights, pointLights, directionalLights, models, cameras);
	return m_scene;
}

void BackPacksDemo::Update(float DeltaTime)
{
	float baseRotAngle = 0.5f;
	float adjustedRotAngle = baseRotAngle / (DeltaTime * 10000);

	for (auto& model : m_scene->getModels())
	{
		auto instances = model->getInstances();

		for (auto& instance : instances)
		{
			instance = glm::rotate(instance, adjustedRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		}
		model->setInstances(instances);
	}
}
