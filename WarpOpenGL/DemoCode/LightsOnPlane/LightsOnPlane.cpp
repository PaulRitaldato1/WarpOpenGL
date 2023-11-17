#include "LightsOnPlane.h"

Ref<Scene> LightOnPlaneDemo::MakeScene(GLFWwindow* window)
{
	FPCamera Camera(glm::vec3(0.0f, 25.0f, 100.0f));
	ModelLoader Loader("Resources/");
	Vector<Ref<Model>> Models;

	Models.push_back(GenerateSphereGrid(1000, 1000));
	Models.push_back(Loader.GenerateGrid(1000, 1000, 100, 100, glm::vec3(0.0f, -4.0f, 0.0f)));
	Models.back()->GetMeshes()[0]->SetTexture(LoadGLTexture("RandomTextures/wood.png", "Resources", "Diffuse"));

	Vector<Pointlight> PointLights = GeneratePointLightGrid(1000, 1000);
	Vector<Spotlight> Spotlights;
	Vector<DirectionalLight> DirectionalLights;

	Vector<FPCamera> Cameras;
	Cameras.push_back(Camera);

	DemoScene = std::make_shared<Scene>(window, "PointLightTest", Spotlights, PointLights, DirectionalLights, Models, Cameras);
	return DemoScene;
}

Vector<Pointlight> LightOnPlaneDemo::GeneratePointLightGrid(int width, int height)
{
	int StartX = -width / 2;
	int StartZ = -height / 2;

	uint RowColFactor = 50;
	int NumRows = height / RowColFactor;
	int NumCols = width / RowColFactor;

	glm::vec3 BasePos = glm::vec3(StartX, 0.0f, StartZ);
	glm::vec3 TempPos = BasePos;
	Vector<Pointlight> Lights;
	for (uint Row = 0; Row < NumRows; Row++)
	{
		for (uint Col = 0; Col < NumCols; Col++)
		{
			float TempX = TempPos.x;// +getRandFloat(-5.0f, 5.0f);
			float TempZ = TempPos.z;// +getRandFloat(-5.0f, 5.0f);
			Lights.emplace_back(glm::vec3(TempX, 0.0, TempZ), GetRandFloat(1.0f, 2.0f), GetRandFloat(25.0f, 50.0f), glm::vec3(GetRandFloat(0.0f, 1.0f), GetRandFloat(0.0f, 1.0f), GetRandFloat(0.0f, 1.0f)));

			TempPos.x += RowColFactor;
		}
		TempPos.x = BasePos.x;
		TempPos.z += RowColFactor;
	}

	return Lights;
}

Ref<Model> LightOnPlaneDemo::GenerateSphereGrid(int width, int height)
{
	glm::mat4 BaseTrans = glm::mat4(1.0);
	Vector<glm::mat4> Transforms;
	Vector<float> Radii;

	int StartX = -width / 2;
	int StartZ = -height / 2;

	uint RowColFactor = 20;
	int NumRows = height / RowColFactor;
	int NumCols = width / RowColFactor;

	glm::vec3 BasePos = glm::vec3(StartX, 0.0f, StartZ);
	glm::vec3 TempPos = BasePos;
	for (uint Row = 0; Row <= NumRows; Row++)
	{
		for (uint Col = 0; Col <= NumCols; Col++)
		{
			float TempX = TempPos.x + GetRandFloat(-3.0f, 3.0f);
			float TempY = TempPos.y + GetRandFloat(-2.0, 2.0);
			float TempZ = TempPos.z + GetRandFloat(-3.0f, 3.0f);
			Transforms.push_back(glm::translate(BaseTrans, glm::vec3(TempX, TempY, TempZ)));
			Radii.push_back(GetRandFloat(2, 5));
			TempPos.x += RowColFactor;
		}
		TempPos.x = BasePos.x;
		TempPos.z += RowColFactor;
	}

	ModelLoader Loader("/");
	return Loader.GenerateSphereInstanced(Radii, Transforms, 100, 100, LoadGLTexture("RandomTextures/StoneTexture.jpg", "Resources", "Diffuse"));
}

void LightOnPlaneDemo::Update(float DeltaTime)
{
}
