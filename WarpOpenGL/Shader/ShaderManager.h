#pragma once
#include <Common/CommonTypes.h>
#include <Shader/Shader.h>

class ShaderManager
{
public:
	Shader getShader(string name) { return m_shaders[name]; }
private:
	HashMap<string, Shader> m_shaders;
};