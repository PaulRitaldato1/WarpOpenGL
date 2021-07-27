#pragma once
#include <Lighting/ILight.h>
#include <Mesh/GeoGen.h>

class DirectionalLight : public ILight
{

public:

	DirectionalLight(glm::vec3 color, glm::vec3 direction, bool castsShadows)
		: m_color(color)
		, m_direction(direction)
		, m_isShadowCaster(castsShadows)
	{
		m_lightVolume = std::make_shared<Model>(GeoGen::CreateDefaultQuad());
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
	
	Ref<Model> m_lightVolume;

};