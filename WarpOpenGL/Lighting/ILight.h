#pragma once
#include <Lighting/LightTypeEnum.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Common/CommonTypes.h>
#include <Lighting/LightShaderParams.h>
#include <Mesh/Model.h>

class ILight
{
public:
	virtual LightType getType() const = 0;
	virtual bool getIsShadowCaster() const = 0;
	virtual glm::vec3 getColor() const = 0;
	virtual LightShaderParams getShaderParams() const = 0;
	virtual Model& getLightVolume() const = 0;
};