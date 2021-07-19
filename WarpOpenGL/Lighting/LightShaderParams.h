#pragma once
#include <glm/glm.hpp>
#include <Common/CommonTypes.h>
#include <Lighting/LightTypeEnum.h>


//this should mirror the light struct on the shader side.
struct LightShaderParams
{
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 color;

	float spotlightAngle;
	float radius;
	float intensity;
	bool enable;
	LightType type;
};