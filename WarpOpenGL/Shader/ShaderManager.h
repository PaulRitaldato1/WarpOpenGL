#pragma once
#include <Common/CommonTypes.h>
#include <Common/Assert.h>
#include <Common/Profiler.h>

#include <Shader/Shader.h>
#include <fstream>
#include <json/json.h>
#include <string_view>

class ShaderManager
{
public:

	//compiles all shaders in specified json file
	ShaderManager(string&& filePath) : m_filePath(std::move(filePath))
	{
		PROFILE_SCOPE("ShaderCompile");

		using json = nlohmann::json;

		std::ifstream file(m_filePath);
		json jFile;

		file >> jFile;

		for (const auto& item : jFile.items())
		{
			string shaderName = item.key();
			ShaderParams params;

			for (const auto& path : item.value().items())
			{
				string key = path.key();
				if (key == "vertPath")
				{
					params.vertPath = path.value();
				}
				else if (key == "fragPath")
				{
					params.fragPath = path.value();
				}
				else if (key == "geoPath")
				{
					params.geoPath = item.value();
				}
				else if (key == "computePath")
				{
					params.computePath = item.value();
				}
			}
	
			m_shaders.push_back(std::make_shared<Shader>(params));
			//map shader names to an index
			m_shaderIndexMap[shaderName] = m_shaders.size() - 1;
		}
	}

	Shader& getShader(string& name) { return *m_shaders[getShaderId(name)]; }
	uint getShaderId(string&& name) { return m_shaderIndexMap[name]; }
	uint getShaderId(string& name) { return m_shaderIndexMap[name]; }
	Shader& getShaderById(uint id) { return *m_shaders[id]; }
private:
	Vector<Ref<Shader>> m_shaders;
	HashMap<string, uint> m_shaderIndexMap;
	string m_filePath;
};