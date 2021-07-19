#pragma once
#include <Common/CommonTypes.h>
#include <glm/glm.hpp>

//mirror of the material defined in shaders
struct Material
{
	glm::vec4 GlobalAmbient;
	
	glm::vec4 AmbientColor;
	glm::vec4 EmissiveColor;
	glm::vec4 DiffuseColor;
	glm::vec4 SpecularColor;
	
	glm::vec4 Reflectance;
	
	float Opacity;
	float SpecularPower;
	float IndexOfRefraction;
	
	bool HasAmbientTexture;
	bool HasEmissiveTexture;
	bool HasDiffuseTexture;
	bool HasSpecularTexture;
	bool HasSpecularPowerTexture;
	bool HasNormalTexture;

	float SpecularScale;
	float AlphaThreshold;
};

//set some good presets so we dont have to manually define everything
enum MaterialPresets
{
	DEFAULT
};

inline HashMap<MaterialPresets, Material> g_MaterialPresets =
{
	{MaterialPresets::DEFAULT, {
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		, glm::vec4(1.0f, 0.5f, 0.31f, 1.0f)
		, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
		, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		, 1.0f
		, 1.0f
		, 1.0f
		, false
		, false
		, true
		, true
		, false
		, true
		, 1.0f
		, 0.01f}}
};