#pragma once
#include <Mesh/Model.h>
#include <Mesh/GeoGen.h>
#include <Lighting/ILight.h>
#include <Mesh/ModelLoader.h>

class Pointlight : public ILight
{

public:
	Pointlight(glm::vec3 position, float intensity, float radius, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f))
		: m_position(position)
		, m_color(color)
		, m_intensity(intensity)
		, m_radius(radius)
	{
		ModelLoader loader("/");
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(radius, radius, radius));
		ModelDesc desc("Resources/planet/planet.obj", false, false, model);
		m_lightVolume = loader.loadModel(desc); //use the planet as a sphere
		m_lightVolume->getMeshes()[0]->setupMesh();
	}

	const Array<glm::mat4, 6>& getViewMatricies()
	{
		
	}

	void setColor(glm::vec3 color) { m_color = color; }


	glm::vec3 getPosition() const { return m_position; }
	void setPosition(glm::vec3 val) { m_position = val; }

	void setIsShadowCaster(bool val) { m_shadowCaster = val; }


	LightType getType() const override
	{
		return LightType::POINTLIGHT;
	}


	bool getIsShadowCaster() const override
	{
		return m_shadowCaster;
	}


	glm::vec3 getColor() const override
	{
		return m_color;
	}


	LightShaderParams getShaderParams() const override
	{
		LightShaderParams params;
		params.color = m_color;
		params.enable = true;
		params.intensity = m_intensity;
		params.position = m_position;
		params.radius = m_radius;
		params.type = LightType::POINTLIGHT;

		return params;
	}

	float getIntensity() const { return m_intensity; }
	void setIntensity(float val) { m_intensity = val; }

	float getRange() const { return m_radius; }
	void setRange(float val) { m_radius = val; }

	const Model& getLightVolume() const override
	{
		return *m_lightVolume;
	}

private:

	bool m_shadowCaster;
	//optinal mesh in case the light can be draw (i.e lightbulb or spotlight mesh)
	Ref<Mesh> m_mesh;
	Ref<Model> m_lightVolume;

	glm::vec3 m_position;
	glm::vec3 m_color;
	float m_intensity;
	float m_radius;
};