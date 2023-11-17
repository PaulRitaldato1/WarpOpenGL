#pragma once
#include <glm/glm.hpp>
#include <Common/CommonTypes.h>
#include <Lighting/LightTypeEnum.h>


//this should mirror the light struct on the shader side.
struct LightShaderParams
{
	glm::vec3 PositionVS;
	glm::vec3 Position;
	glm::vec3 Direction;

	glm::vec3 Color;

	float SpotlightAngle;
	float Radius;
	float Intensity;
	bool bEnable;
	LightType Type;
};