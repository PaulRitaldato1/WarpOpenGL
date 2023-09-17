#pragma once

#include <DemoCode/Demo.h>

#include <Common/CommonTypes.h>
#include <Common/CommonMath.h>

#include <Includes/glm/glm.hpp>

#include <Lighting/Pointlight.h>
#include <Mesh/Model.h>
#include <Mesh/ModelLoader.h>

class LightOnPlaneDemo : public Demo
{
public:

	Ref<Scene> MakeScene(GLFWwindow* window) override;

private:
	
	Vector<Pointlight> generatePointLightGrid(int width, int height);

	// Inherited via Demo
	virtual void Update(float DeltaTime) override;

	Ref<Model> generateSphereGrid(int width, int height);
};


