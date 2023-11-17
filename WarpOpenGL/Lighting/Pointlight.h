#pragma once
#include <Mesh/Model.h>
#include <Mesh/GeoGen.h>
#include <Lighting/ILight.h>
#include <Mesh/ModelLoader.h>

class Pointlight : public ILight
{

public:
	Pointlight(glm::vec3 position, float intensity, float radius, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f))
		: Position(position)
		, Color(color)
		, Intensity(intensity)
		, Radius(radius)
	{
		ModelLoader Loader("/");
		//glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, position);
		//model = glm::scale(model, glm::vec3(radius, radius, radius));
		LightVolume = Loader.GenerateSphere(radius, 10, 10, position);
		//m_lightVolume = loader.loadModel(desc); //use the planet as a sphere
		//m_lightVolume->getMeshes()[0]->setupMesh();
	}

	const Array<glm::mat4, 6>& GetViewMatricies()
	{
		
	}

	void SetColor(glm::vec3 color) { Color = color; }


	glm::vec3 GetPosition() const { return Position; }
	void SetPosition(glm::vec3 val) { Position = val; }

	void SetIsShadowCaster(bool val) { bIsShadowCaster = val; }


	LightType GetType() const override
	{
		return LightType::POINTLIGHT;
	}


	bool GetIsShadowCaster() const override
	{
		return bIsShadowCaster;
	}


	glm::vec3 GetColor() const override
	{
		return Color;
	}


	LightShaderParams GetShaderParams() const override
	{
		LightShaderParams Params;
		Params.Color = Color;
		Params.bEnable = true;
		Params.Intensity = Intensity;
		Params.Position = Position;
		Params.Radius = Radius;
		Params.Type = LightType::POINTLIGHT;

		return Params;
	}

	float GetIntensity() const { return Intensity; }
	void SetIntensity(float val) { Intensity = val; }

	float GetRange() const { return Radius; }
	void SetRange(float val) { Radius = val; }

	Model& GetLightVolume() const override
	{
		return *LightVolume;
	}

	void SetDistanceToCamera(float value) { DistanceToCamera = value; }
	float GetDistanceToCamera() const { return DistanceToCamera; }
private:

	bool bIsShadowCaster;
	//optinal mesh in case the light can be draw (i.e lightbulb or spotlight mesh)
	Ref<WarpMesh> Mesh;
	Ref<Model> LightVolume;

	glm::vec3 Position;
	glm::vec3 Color;
	float Intensity;
	float Radius;

	float DistanceToCamera;
};