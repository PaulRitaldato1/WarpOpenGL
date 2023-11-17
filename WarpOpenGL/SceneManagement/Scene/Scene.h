#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Common/CommonTypes.h>
#include <Lighting/Pointlight.h>
#include <Lighting/Spotlight.h>
#include <Lighting/DirectionalLight.h>
#include <Mesh/Model.h>
#include <Cameras/FPCamera.h>
#include <algorithm>

struct FarthestDistanceToCameraPointlight
{
	bool operator()(const Pointlight& l1, const Pointlight& l2)
	{
		return (l1.GetDistanceToCamera() > l2.GetDistanceToCamera());
	}
};

class Scene
{
public:

	//expensive constructor, but this should only be run during setup, or loading, not per frame
	Scene(GLFWwindow* window, string name, Vector<Spotlight> spotlights, Vector<Pointlight> pointlights, Vector<DirectionalLight> directionalLights, Vector<Ref<Model>> models, Vector<FPCamera> cameras)
		: Window(window)
		, Name(name)
		, Spotlights(spotlights)
		, Pointlights(pointlights)
		, DirectionalLights(directionalLights)
		, Models(models)
		, Cameras(cameras)
		, ActiveCameraIndex(0)
		, bWindowChanged(false)
	{
		DYNAMIC_ASSERT(Cameras.size() > 0, "No Camera(s) provided to scene, creating a default one");
		DYNAMIC_ASSERT(Spotlights.size() > 0 || Pointlights.size() > 0 || DirectionalLights.size() > 0, "No lights provided to the scene. Render may be dark.");
		DYNAMIC_ASSERT(Models.size() > 0, "No models in the current scene");

		if (cameras.size() == 0)
		{
			//TODO create default camera
		}
	}

	void Update(float delta)
	{	

		//float baseRotAngle = 1.0f;
		//float adjustedRotAngle = baseRotAngle/(delta * 10000);
		//for (const auto& model : m_models)
		//{
		//	if (!model->getIsInstanced())
		//	{
		//		//auto currentTransform = model->getTransform();
		//		//model->setTransform(glm::rotate(currentTransform, adjustedRotAngle, glm::vec3(0.0f, 1.0f, 0.0f)));
		//	}
		//	else
		//	{
		//		auto instances = model->getInstances();
		//		for (auto& instance : instances)
		//		{
		//			instance = glm::rotate(instance, adjustedRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		//		}

		//		model->setInstances(instances);
		//	}
		//}

		//m_pointlights[0].setPosition(glm::vec3(sin(glfwGetTime()) * 10, 0.0f, 0.0f));
		//for (auto& light : m_pointlights)
		//{
		//	glm::vec3 newPos = glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime()));
		//	light.setPosition(newPos);
		//	glm::mat4 trans = glm::mat4(1.0f);
		//	light.getLightVolume().setTransform(glm::translate(light.getLightVolume().getTransform(), newPos));
		//}
	}

	void AddPointlight(Pointlight light) { Pointlights.push_back(light); }
	void AddSpotlight(Spotlight light) { Spotlights.push_back(light); }
	void AddModel(Ref<Model> model) { Models.push_back(model); }
	void AddCamera(FPCamera camera) { Cameras.push_back(camera); }

	const Vector<Pointlight>& GetPointlights() { return Pointlights; }
	const Vector<Spotlight>& GetSpotlights() { return Spotlights; }
	const Vector<DirectionalLight>& GetDirectionalLights() { return DirectionalLights; }
	const Vector<Ref<Model>>& GetModels() { return Models; }

	const Vector<FPCamera>& GetCameras() { return Cameras; }

	FPCamera& GetActiveCamera() { return Cameras[ActiveCameraIndex]; }
	
	uint GetSpotlightCount() { return Spotlights.size(); }
	uint GetPointlightCount() { return Pointlights.size(); }
	uint GetModelCount() { return Models.size(); }
	uint GetCameraCount() { return Cameras.size(); }

	Tuple<int, int> GetWindowSize()
	{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(Window, &width, &height);
		return Tuple<int, int>(width, height);
	}

	bool HasWindowChanged() 
	{ 
		if (bWindowChanged)
		{
			bWindowChanged = false;
			return true;
		}
		else
			return false;
	}

	void SetWindowChanged(bool bToggle) { bWindowChanged = bToggle; }

private:
	GLFWwindow* Window;
	string Name;
	uint ActiveCameraIndex;
	Vector<Spotlight> Spotlights;
	Vector<Pointlight> Pointlights;
	Vector<DirectionalLight> DirectionalLights;
	Vector<Ref<Model>> Models;
	Vector<FPCamera> Cameras;
	bool bWindowChanged;
};
