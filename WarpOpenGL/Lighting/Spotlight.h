#pragma once
#include <Mesh/Mesh.h>
#include <Lighting/ILight.h>

class Spotlight : public ILight
{
public:

	Spotlight(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f), float radius = 10.0f, float yaw = 0.0f, float pitch = 0.0f) :
		Position(position)
		, WorldUp(up)
		, Color(color)
		, Radius(radius)
		, Yaw(yaw)
		, Pitch(pitch)
	{
		// TODO set light volume as cone
		UpdateVectors();
	}

	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	glm::vec3 GetPosition() const { return Position; }
	void SetPosition(glm::vec3 val) { Position = val; }
	
	glm::vec3 GetFront() const { return Front; }
	void SetFront(glm::vec3 val) { Front = val; }
	
	glm::vec3 GetUp() const { return Up; }
	void SetUp(glm::vec3 val) { Up = val; }
	
	glm::vec3 GetRight() const { return Right; }
	void SetRight(glm::vec3 val) { Right = val; }
	
	glm::vec3 GetWorldUp() const { return WorldUp; }
	void SetWorldUp(glm::vec3 val) { WorldUp = val; }
	
	glm::vec3 GetColor() const override { return Color; }
	void SetColor(glm::vec3 val) { Color = val; }
	
	float GetYaw() const { return Yaw; }
	void SetYaw(float val) { Yaw = val; }
	
	float GetPitch() const { return Pitch; }
	void SetPitch(float val) { Pitch = val; }

	void SetShadowCaster(bool val) { bIsShadowCaster = val; }

	LightType GetType() const override { return LightType::SPOTLIGHT; }

	bool GetIsShadowCaster() const override
	{
		return bIsShadowCaster;
	}


	LightShaderParams GetShaderParams() const override
	{
		LightShaderParams Params;
		Params.Color = Color;
		Params.Direction = Position + Front;
		Params.bEnable = true;
		Params.Intensity = Intensity;
		Params.Position = Position;
		Params.Radius = Range;
		Params.SpotlightAngle = Angle;
		Params.Type = LightType::SPOTLIGHT;

		return Params;
	}

	Model& GetLightVolume() const override { return *LightVolume; }
private:

	void UpdateVectors()
	{
		glm::vec3 FrontCalc;
		FrontCalc.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		FrontCalc.y = sin(glm::radians(Pitch));
		FrontCalc.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		Front = glm::normalize(FrontCalc);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}

	bool bIsShadowCaster;

	//optional mesh in case the light can be draw (i.e lightbulb or spotlight mesh)
	Ref<WarpMesh> Mesh;
	Ref<Model> LightVolume;

	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	glm::vec3 Color;
	float Range;
	float Intensity;
	float Angle;

	bool bEnabled;
	float Yaw;
	float Pitch;
	float Radius;
};