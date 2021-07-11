#pragma once
#include <Common/CommonTypes.h>
#include <Lights/Pointlight.h>
#include <Lights/Spotlight.h>
#include <Mesh/Model.h>
#include <Cameras/FPCamera.h>

class Scene
{
public:

	//expensive constructor, but this should only be run during setup, or loading, not per frame
	Scene(string name, Vector<Spotlight> spotlights, Vector<Pointlight> pointlights, Vector<Ref<Model>> models, Vector<FPCamera> cameras)
		: m_name(name)
		, m_spotlights(spotlights)
		, m_pointlights(pointlights)
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

	void addPointlight(Pointlight light) { m_pointlights.push_back(light); }
	void addSpotlight(Spotlight light) { m_spotlights.push_back(light); }
	void addModel(Ref<Model> model) { m_models.push_back(model); }
	void addCamera(FPCamera camera) { m_cameras.push_back(camera); }

	const Vector<Pointlight>& getPointlights() { return m_pointlights; }
	const Vector<Spotlight>& getSpotlights() { return m_spotlights; }
	const Vector<Ref<Model>>& getModels() { return m_models; }
	const Vector<FPCamera>& getCamera() { return m_cameras; }

	const FPCamera& getActiveCamera() { return m_cameras[m_activeCameraIndex]; }
	
	uint getSpotlightCount() { m_spotlights.size(); }
	uint getPointlightCount() { m_pointlights.size(); }
	uint getModelCount() { m_models.size(); }
	uint getCameraCount() { m_cameras.size(); }

private:
	string m_name;
	uint m_activeCameraIndex;
	Vector<Spotlight> m_spotlights;
	Vector<Pointlight> m_pointlights;
	Vector<Ref<Model>> m_models;
	Vector<FPCamera> m_cameras;
};