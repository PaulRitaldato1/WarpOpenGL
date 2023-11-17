#pragma once
#include <Lighting/ILight.h>
#include <Mesh/GeoGen.h>

class DirectionalLight : public ILight
{

public:

	DirectionalLight(glm::vec3 color, glm::vec3 direction, float intensity, bool castsShadows)
		: Color(color)
		, Direction(direction)
		, Intensity(intensity)
		, bIsShadowCaster(castsShadows)
	{
		ModelLoader Loader("/");
		LightVolume = Loader.GenerateQuad();
	}

	LightType GetType() const override
	{
		return LightType::DIRECTIONAL;
	}


	bool GetIsShadowCaster() const override
	{
		return bIsShadowCaster;
	}

	void SetIsShadowCaster(bool toggle) { bIsShadowCaster = toggle; }

	glm::vec3 GetColor() const override
	{
		return Color;
	}


	LightShaderParams GetShaderParams() const override
	{
		LightShaderParams Params;
		Params.Color = Color;
		Params.Direction = Direction;
		Params.Intensity = Intensity;
		Params.Type = LightType::DIRECTIONAL;
		return Params;
	}

	Model& GetLightVolume() const override
	{
		return *LightVolume;
	}

private:
	bool bIsShadowCaster;

	glm::vec3 Color;
	glm::vec3 Direction;
	float Intensity;

	Ref<Model> LightVolume;

};