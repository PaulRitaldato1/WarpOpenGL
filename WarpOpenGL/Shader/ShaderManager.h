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
	ShaderManager(string&& filePath) : FilePath(std::move(filePath))
	{
		PROFILE_SCOPE("ShaderCompile");

		using json = nlohmann::json;

		std::ifstream File(FilePath);
		FATAL_ASSERT(File.is_open(), "File at path " + FilePath + " failed to open!");

		json jFile;

		File >> jFile;

		for (const auto& FileToken : jFile.items())
		{
			string ShaderName = FileToken.key();
			ShaderParams Params;

			for (const auto& Path : FileToken.value().items())
			{
				string Key = Path.key();
				if (Key == "vertPath")
				{
					Params.VertPath = Path.value();
				}
				else if (Key == "fragPath")
				{
					Params.FragPath = Path.value();
				}
				else if (Key == "geoPath")
				{
					Params.GeoPath = FileToken.value();
				}
				else if (Key == "computePath")
				{
					Params.ComputePath = FileToken.value();
				}
			}
			
			Params.Name = ShaderName;
			Shaders.push_back(std::make_shared<GLSLShader>(Params));
			//map shader names to an index
			ShaderIndexMap[ShaderName] = Shaders.size() - 1;
		}
	}

	GLSLShader& GetShader(string& name) { return *Shaders[GetShaderId(name)]; }
	uint GetShaderId(string&& name) { return ShaderIndexMap[name]; }
	uint GetShaderId(string& name) { return ShaderIndexMap[name]; }
	GLSLShader& GetShaderById(uint id) { return *Shaders[id]; }
	Vector<Ref<GLSLShader>>& GetShaders() { return Shaders; }
private:
	Vector<Ref<GLSLShader>> Shaders;
	HashMap<string, uint> ShaderIndexMap;
	string FilePath;
};