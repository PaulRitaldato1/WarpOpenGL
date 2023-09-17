#include "LightsOnPlane.h"

Ref<Scene> LightOnPlaneDemo::MakeScene(GLFWwindow* window)
{
	FPCamera camera(glm::vec3(0.0f, 25.0f, 100.0f));
	ModelLoader loader("Resources/");
	Vector<Ref<Model>> models;

	models.push_back(generateSphereGrid(1000, 1000));
	models.push_back(loader.generateGrid(1000, 1000, 100, 100, glm::vec3(0.0f, -4.0f, 0.0f)));
	models.back()->getMeshes()[0]->setTexture(loadGLTexture("RandomTextures/wood.png", "Resources", "Diffuse"));

	Vector<Pointlight> pointLights = generatePointLightGrid(1000, 1000);
	Vector<Spotlight> spotlights;
	Vector<DirectionalLight> directionalLights;

	Vector<FPCamera> cameras;
	cameras.push_back(camera);

	m_scene = std::make_shared<Scene>(window, "PointLightTest", spotlights, pointLights, directionalLights, models, cameras);
	return m_scene;
}

Vector<Pointlight> LightOnPlaneDemo::generatePointLightGrid(int width, int height)
{
	int startX = -width / 2;
	int startZ = -height / 2;

	uint rowColFactor = 50;
	int numRows = height / rowColFactor;
	int numCols = width / rowColFactor;

	glm::vec3 basePos = glm::vec3(startX, 0.0f, startZ);
	glm::vec3 tempPos = basePos;
	Vector<Pointlight> lights;
	for (uint row = 0; row < numRows; row++)
	{
		for (uint col = 0; col < numCols; col++)
		{
			float tempX = tempPos.x;// +getRandFloat(-5.0f, 5.0f);
			float tempZ = tempPos.z;// +getRandFloat(-5.0f, 5.0f);
			lights.emplace_back(glm::vec3(tempX, 0.0, tempZ), GetRandFloat(1.0f, 2.0f), GetRandFloat(25.0f, 50.0f), glm::vec3(GetRandFloat(0.0f, 1.0f), GetRandFloat(0.0f, 1.0f), GetRandFloat(0.0f, 1.0f)));

			tempPos.x += rowColFactor;
		}
		tempPos.x = basePos.x;
		tempPos.z += rowColFactor;
	}

	return lights;
}

Ref<Model> LightOnPlaneDemo::generateSphereGrid(int width, int height)
{
	glm::mat4 baseTrans = glm::mat4(1.0);
	Vector<glm::mat4> transforms;
	Vector<float> radii;

	int startX = -width / 2;
	int startZ = -height / 2;

	uint rowColFactor = 20;
	int numRows = height / rowColFactor;
	int numCols = width / rowColFactor;

	glm::vec3 basePos = glm::vec3(startX, 0.0f, startZ);
	glm::vec3 tempPos = basePos;
	for (uint row = 0; row <= numRows; row++)
	{
		for (uint col = 0; col <= numCols; col++)
		{
			float tempX = tempPos.x + GetRandFloat(-3.0f, 3.0f);
			float tempY = tempPos.y + GetRandFloat(-2.0, 2.0);
			float tempZ = tempPos.z + GetRandFloat(-3.0f, 3.0f);
			transforms.push_back(glm::translate(baseTrans, glm::vec3(tempX, tempY, tempZ)));
			radii.push_back(GetRandFloat(2, 5));
			tempPos.x += rowColFactor;
		}
		tempPos.x = basePos.x;
		tempPos.z += rowColFactor;
	}

	ModelLoader loader("/");
	return loader.generateSphereInstanced(radii, transforms, 100, 100, loadGLTexture("RandomTextures/StoneTexture.jpg", "Resources", "Diffuse"));
}

void LightOnPlaneDemo::Update(float DeltaTime)
{
}
