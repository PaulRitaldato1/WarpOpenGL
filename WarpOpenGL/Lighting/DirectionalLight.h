#pragma once
#include <Lighting/ILight.h>
#include <Mesh/GeoGen.h>

class DirectionalLight : public ILight
{

public:

	DirectionalLight(glm::vec3 color, glm::vec3 direction, float intensity, bool castsShadows)
		: m_color(color)
		, m_direction(direction)
		, m_intensity(intensity)
		, m_isShadowCaster(castsShadows)
	{
		ModelLoader loader("/");
		m_lightVolume = loader.generateQuad();
	}

	LightType getType() const override
	{
		return LightType::DIRECTIONAL;
	}


	bool getIsShadowCaster() const override
	{
		return m_isShadowCaster;
	}

	void setIsShadowCaster(bool toggle) { m_isShadowCaster = toggle; }

	glm::vec3 getColor() const override
	{
		return m_color;
	}


	LightShaderParams getShaderParams() const override
	{
		LightShaderParams params;
		params.color = m_color;
		params.direction = m_direction;
		params.intensity = m_intensity;
		params.type = LightType::DIRECTIONAL;
		return params;
	}

	const Model& getLightVolume() const override
	{
		return *m_lightVolume;
	}

private:
	bool m_isShadowCaster;

	glm::vec3 m_color;
	glm::vec3 m_direction;
	float m_intensity;

	Ref<Model> m_lightVolume;

};