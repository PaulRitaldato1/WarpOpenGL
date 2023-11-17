#include "Asteroids.h"

#include <Common/CommonTypes.h>
#include <Common/CommonMath.h>

Ref<Scene> Asteroids::MakeScene(GLFWwindow* Window)
{
	ModelLoader Loader("Resources/");
	FPCamera camera(glm::vec3(0.0f, 25.0f, 250.0f), glm::vec3(0.0, 1.0, 0.0), -90.0, 0);

	Vector<glm::mat4> ModelMatrices = GenerateAsteroidPositions(10000);
	glm::mat4 Base = glm::mat4(1.0f);

	Vector<ModelDesc> ModelDescs;
	ModelDescs.emplace_back("Resources/planet/planet.obj", false, true, glm::scale(Base, glm::vec3(10, 10, 10)));
	ModelDescs.emplace_back("Resources/rock/rock.obj", false, true, ModelMatrices);

	Vector<Ref<Model>> Models = Loader.LoadModelsAsync(ModelDescs, 2);

	for (int i = 0; i < Models.size(); i++)
	{
		if (!Models[i]->GetIsInstanced())
		{
			Planet = Models[i];
			break;
		}
	}

	Vector<Pointlight> PointLights;
	Vector<Spotlight> Spotlights;
	Vector<DirectionalLight> DirectionalLights;
	DirectionalLights.emplace_back(glm::vec3(1.0, 1.0, 1.0), glm::vec3(0.0, -1.0, 0.0), .75, false);

	Vector<FPCamera> Cameras;
	Cameras.push_back(camera);

	DemoScene = std::make_shared<Scene>(Window, "Asteroids Test", Spotlights, PointLights, DirectionalLights, Models, Cameras);
	return DemoScene;
}

Vector<glm::mat4> Asteroids::GenerateAsteroidPositions(uint Amount)
{
	Vector<glm::mat4> ModelMatrices;

	float Offset = 25.0f;
	for (unsigned int i = 0; i < Amount; i++)
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float Angle = (float)i / (float)Amount * 360.0f;
		float Displacement = (GetRandFloat(0.0f, 2 * Offset * 100)) / 100.0f - Offset;
		float x = sin(Angle) * Radius + Displacement;
		Displacement = (GetRandFloat(0.0f, 2 * Offset * 100)) / 100.0f - Offset;
		float y = Displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		Displacement = (GetRandFloat(0.0f, 2 * Offset * 100)) / 100.0f - Offset;
		float z = cos(Angle) * Radius + Displacement;
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float Scale = (GetRandFloat(0.0f, 20.0f)) / 100.0f + 0.05;
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(Scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float RotAngle = (GetRandFloat(0.0f, 360.0f));
		ModelMatrix = glm::rotate(ModelMatrix, RotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		ModelMatrices.push_back(ModelMatrix);
	}

	return ModelMatrices;
}

void Asteroids::Update(float DeltaTime)
{
	float BaseRotAngle = 1.0f;
	float AdjustedLocalRotAngle = BaseRotAngle / (DeltaTime * 10000);

	glm::vec3 PlanetPos = Planet->GetPosition();

	for (const auto& Model : DemoScene->GetModels())
	{
		if (!Model->GetIsInstanced())
		{
			auto CurrentTransform = Model->GetTransform();
			Model->SetTransform(glm::rotate(CurrentTransform, AdjustedLocalRotAngle / (DeltaTime * 1000), glm::vec3(0.0f, 1.0f, 0.0f)));
		}
		else
		{
			auto Instances = Model->GetInstances();
			for (auto& Instance : Instances)
			{
				Instance = glm::rotate(Instance, AdjustedLocalRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
				
				glm::vec3 CurrentPos = glm::vec3(Instance[3]);
				
				float deltaX = CurrentPos.x - PlanetPos.x;
				float deltaZ = CurrentPos.z - PlanetPos.z;

				float CurrentAngleRad = atan2(deltaZ, deltaX);

				float CurrentAngleDeg = glm::degrees(CurrentAngleRad);
				
				float NewAngleDeg = CurrentAngleDeg + AdjustedLocalRotAngle / 100000.0f;

				if (NewAngleDeg > 360.0f)
				{
					NewAngleDeg = 0;
				}

				float NewAngleRad = glm::radians(NewAngleDeg);


				float InstanceRadius = glm::distance(CurrentPos, PlanetPos);

				glm::vec3 NewPos = glm::vec3(sin(NewAngleDeg) * InstanceRadius, CurrentPos.y, cos(NewAngleDeg) * InstanceRadius);

				Instance[3] = glm::vec4(NewPos, 1.0);

			}

			Model->SetInstances(Instances);
		}
	}
}
