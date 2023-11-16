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

	float m_radius = 150.0f;
	float m_orbitAngle = 0.0f;
	float m_planetRot = 0.1f;
	Ref<Model> m_planet;
	Vector<glm::mat4> generateAsteroidPositions(uint amount);
};