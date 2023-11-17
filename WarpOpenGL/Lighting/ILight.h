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
	virtual LightType GetType() const = 0;
	virtual bool GetIsShadowCaster() const = 0;
	virtual glm::vec3 GetColor() const = 0;
	virtual LightShaderParams GetShaderParams() const = 0;
	virtual Model& GetLightVolume() const = 0;
};