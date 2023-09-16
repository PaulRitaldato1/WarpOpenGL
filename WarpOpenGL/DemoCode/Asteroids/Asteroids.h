#pragma once

#include <DemoCode/Demo.h>

#include <Common/CommonTypes.h>
#include <Common/CommonMath.h>

#include <Includes/glm/glm.hpp>

#include <Lighting/Pointlight.h>
#include <Mesh/Model.h>
#include <Mesh/ModelLoader.h>

class Asteroids : public Demo
{
public:

	virtual Ref<Scene> MakeScene(GLFWwindow* window) override
	{
		ModelLoader loader("Resources/");
		FPCamera camera(glm::vec3(0.0f, 25.0f, 250.0f), glm::vec3(0.0, 1.0, 0.0), -90.0, 0);

		Vector<glm::mat4> modelMatrices = generateAsteroidPositions(50000);
		glm::mat4 base = glm::mat4(1.0f);

		Vector<ModelDesc> modelDescs;
		modelDescs.emplace_back("Resources/planet/planet.obj", false, true, glm::scale(base, glm::vec3(10,10,10)));
		modelDescs.emplace_back("Resources/rock/rock.obj", false, true, modelMatrices);

		Vector<Ref<Model>> models = loader.loadModelsAsync(modelDescs, 2);

		Vector<Pointlight> pointLights;
		Vector<Spotlight> spotlights;
		Vector<DirectionalLight> directionalLights;
		directionalLights.emplace_back(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, -1.0, 0.0), .75, false);

		Vector<FPCamera> cameras;
		cameras.push_back(camera);

		return std::make_shared<Scene>(window, "Asteroids Test", spotlights, pointLights, directionalLights, models, cameras);
	}

private:

	Vector<glm::mat4> generateAsteroidPositions(uint amount)
	{
		Vector<glm::mat4> modelMatrices;

		float radius = 150.0;
		float offset = 25.0f;
		for (unsigned int i = 0; i < amount; i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			// 1. translation: displace along circle with 'radius' in range [-offset, offset]
			float angle = (float)i / (float)amount * 360.0f;
			float displacement = (GetRandFloat(0.0f, 2 * offset * 100)) / 100.0f - offset;
			float x = sin(angle) * radius + displacement;
			displacement = (GetRandFloat(0.0f, 2 * offset * 100)) / 100.0f - offset;
			float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
			displacement = (GetRandFloat(0.0f, 2 * offset * 100)) / 100.0f - offset;
			float z = cos(angle) * radius + displacement;
			model = glm::translate(model, glm::vec3(x, y, z));

			// 2. scale: Scale between 0.05 and 0.25f
			float scale = (GetRandFloat(0.0f, 20.0f)) / 100.0f + 0.05;
			model = glm::scale(model, glm::vec3(scale));

			// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
			float rotAngle = (GetRandFloat(0.0f, 360.0f));
			model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

			// 4. now add to list of matrices
			modelMatrices.push_back(model);
		}

		return modelMatrices;
	}
};