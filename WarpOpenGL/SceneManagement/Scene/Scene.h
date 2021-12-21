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
		return (l1.getDistanceToCamera() > l2.getDistanceToCamera());
	}
};

class Scene
{
public:

	//expensive constructor, but this should only be run during setup, or loading, not per frame
	Scene(string name, Vector<Spotlight> spotlights, Vector<Pointlight> pointlights, Vector<DirectionalLight> directionalLights, Vector<Ref<Model>> models, Vector<FPCamera> cameras)
		: m_name(name)
		, m_spotlights(spotlights)
		, m_pointlights(pointlights)
		, m_directionalLights(directionalLights)
		, m_models(models)
		, m_cameras(cameras)
		, m_activeCameraIndex(0)
	{
		DYNAMIC_ASSERT(m_cameras.size() > 0, "No Camera(s) provided to scene, creating a default one");
		DYNAMIC_ASSERT(m_spotlights.size() > 0 || m_pointlights.size() > 0, "No lights provided to the scene. Render may be dark.");
		DYNAMIC_ASSERT(m_models.size() > 0, "No models in the current scene");

		if (cameras.size() == 0)
		{
			//TODO create default camera
		}
	}

	void update(float delta)
	{	

		float baseRotAngle = 1.0f;
		float adjustedRotAngle = baseRotAngle/(delta * 10000);
		for (const auto& model : m_models)
		{
			if (!model->getIsInstanced())
			{
				//auto currentTransform = model->getTransform();
				//model->setTransform(glm::rotate(currentTransform, adjustedRotAngle, glm::vec3(0.0f, 1.0f, 0.0f)));
			}
			else
			{
				auto instances = model->getInstances();
				for (auto& instance : instances)
				{
					instance = glm::rotate(instance, adjustedRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
				}

				model->setInstances(instances);
			}
		}

		if (m_cameras[m_activeCameraIndex].hasMoved())
		{

			for (auto& light : m_pointlights)
			{
				light.setDistanceToCamera(glm::distance(m_cameras[m_activeCameraIndex].getPosition(), light.getPosition()));
			}

			std::sort(m_pointlights.begin(), m_pointlights.end(), FarthestDistanceToCameraPointlight());
		}
		//m_pointlights[0].setPosition(glm::vec3(sin(glfwGetTime()) * 10, 0.0f, 0.0f));
	}

	void addPointlight(Pointlight light) { m_pointlights.push_back(light); }
	void addSpotlight(Spotlight light) { m_spotlights.push_back(light); }
	void addModel(Ref<Model> model) { m_models.push_back(model); }
	void addCamera(FPCamera camera) { m_cameras.push_back(camera); }

	const Vector<Pointlight>& getPointlights() { return m_pointlights; }
	const Vector<Spotlight>& getSpotlights() { return m_spotlights; }
	const Vector<DirectionalLight>& getDirectionalLights() { return m_directionalLights; }
	const Vector<Ref<Model>>& getModels() { return m_models; }

	const Vector<FPCamera>& getCameras() { return m_cameras; }

	FPCamera& getActiveCamera() { return m_cameras[m_activeCameraIndex]; }
	
	uint getSpotlightCount() { m_spotlights.size(); }
	uint getPointlightCount() { m_pointlights.size(); }
	uint getModelCount() { m_models.size(); }
	uint getCameraCount() { m_cameras.size(); }

private:
	string m_name;
	uint m_activeCameraIndex;
	Vector<Spotlight> m_spotlights;
	Vector<Pointlight> m_pointlights;
	Vector<DirectionalLight> m_directionalLights;
	Vector<Ref<Model>> m_models;
	Vector<FPCamera> m_cameras;
};
