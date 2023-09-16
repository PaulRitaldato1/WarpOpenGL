#pragma once

#include <Common/CommonTypes.h>
#include <SceneManagement/Scene/Scene.h>

class Demo
{
public:
	Demo() {}
	virtual ~Demo() {}
	virtual Ref<Scene> MakeScene(GLFWwindow* window) = 0;
};