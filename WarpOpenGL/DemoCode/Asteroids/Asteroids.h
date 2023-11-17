#pragma once

#include <DemoCode/Demo.h>

#include <Includes/glm/glm.hpp>

#include <Lighting/Pointlight.h>
#include <Mesh/Model.h>
#include <Mesh/ModelLoader.h>

class Asteroids : public Demo
{
public:

	virtual Ref<Scene> MakeScene(GLFWwindow* window);

	virtual void Update(float DeltaTime) override;
private:

	float Radius = 150.0f;
	Ref<Model> Planet;
	Vector<glm::mat4> GenerateAsteroidPositions(uint amount);
};