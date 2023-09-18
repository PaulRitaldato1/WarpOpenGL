#pragma once

#include <DemoCode/Demo.h>

class BackPacksDemo : public Demo
{
public:

	virtual Ref<Scene> MakeScene(GLFWwindow* window) override;
	virtual void Update(float DeltaTime) override;
};