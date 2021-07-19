#pragma once
#include <Lighting/ILight.h>

class DirectionalLight : public ILight
{

public:

	DirectionalLight(glm:vec3 color, bool castsShadows)
		: m_color(color)
		, m_isShadowCaster(castsShadows)
	{
		//TODO set volume to full screen quad
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


	LightShaderParams getShaderParams() override
	{
		return LightShaderParams();
	}


	const Mesh& getLightVolume() const override
	{
		return *m_lightVolume;
	}

private:
	bool m_isShadowCaster;

	glm::vec3 m_color;

	Ref<Mesh> m_lightVolume;

};