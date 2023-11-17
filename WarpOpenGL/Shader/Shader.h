#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Common/CommonTypes.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Common/Assert.h>

struct ShaderParams
{
	string Name;
	string VertPath;
	string FragPath;
	string GeoPath;
	string ComputePath;
};

class GLSLShader
{
public:


	GLSLShader(string vertexPath, string fragmentPath, string geometryPath = "") :
		VertPath(vertexPath)
		, FragPath(fragmentPath)
		, GeoPath(geometryPath)
	{
		Compile();
	}

	GLSLShader(ShaderParams& params) : Name(params.Name)
	{

		//might not need this since string will be "" and ignored during compile anyway
		if (params.VertPath.size() > 0)
		{
			VertPath = params.VertPath;
		}
		if (params.FragPath.size() > 0)
		{
			FragPath = params.FragPath;
		}
		if (params.GeoPath.size() > 0)
		{
			GeoPath = params.GeoPath;
		}
		if (params.ComputePath.size() > 0)
		{
			ComputePath = params.ComputePath;
		}

		Compile();
	}

	GLSLShader(string computePath) : ComputePath(computePath)
	{
		string cCode;

		std::ifstream cFile;

		cFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			cFile.open(ComputePath);
			std::stringstream cStream;
			cStream << cFile.rdbuf();
			cFile.close();
			cCode = cStream.str();
		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
		}

		const char* cShaderCode = cCode.c_str();

		uint computeId = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeId, 1, &cShaderCode, NULL);
		glCompileShader(computeId);
		CheckCompileErrors(computeId, ShaderType::COMPUTE);

		Id = glCreateProgram();
		glAttachShader(Id, computeId);
		glLinkProgram(Id);
		CheckCompileErrors(Id, ShaderType::PROGRAM);

		glDeleteShader(computeId);
	}

	void Recompile()
	{
		Unbind();
		glDeleteShader(Id);
		Compile();
	}

	//separate gl calls into their own function
	void Compile()
	{
		string vCode;
		string fCode;
		string gCode;

		std::ifstream vFile;
		std::ifstream fFile;
		std::ifstream gFile;

		vFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);


		try
		{
			vFile.open(VertPath);
			fFile.open(FragPath);

			std::stringstream vStream;
			std::stringstream fStream;
			vStream << vFile.rdbuf();
			fStream << fFile.rdbuf();

			vFile.close();
			fFile.close();

			vCode = vStream.str();
			fCode = fStream.str();

			if (GeoPath != "")
			{
				gFile.open(GeoPath);
				std::stringstream gStream;
				gStream << gFile.rdbuf();
				gFile.close();
				gCode = gStream.str();
			}

		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ for shader " << Name << ": "<<  e.what() << std::endl;
		}

		const char* vShaderCode = vCode.c_str();
		const char* fShaderCode = fCode.c_str();

		uint VertId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(VertId, 1, &vShaderCode, NULL);
		glCompileShader(VertId);
		CheckCompileErrors(VertId, ShaderType::VERTEX);

		uint FragId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(FragId, 1, &fShaderCode, NULL);
		glCompileShader(FragId);
		CheckCompileErrors(FragId, ShaderType::FRAGMENT);

		uint GeoId;
		if (GeoPath != "")
		{
			const char* gShaderCode = gCode.c_str();
			GeoId = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(GeoId, 1, &gShaderCode, NULL);
			glCompileShader(GeoId);
			CheckCompileErrors(GeoId, ShaderType::GEOMETRY);
		}

		Id = glCreateProgram();
		glAttachShader(Id, VertId);
		glAttachShader(Id, FragId);
		if (GeoPath != "")
			glAttachShader(Id, GeoId);
		glLinkProgram(Id);
		CheckCompileErrors(Id, ShaderType::PROGRAM);

		glDeleteShader(VertId);
		glDeleteShader(FragId);
		if (GeoPath != "")
			glDeleteShader(GeoId);

	}

	void Bind()
	{
		glUseProgram(Id);
	}

	void Unbind()
	{
		glUseProgram(0);
	}

	template<typename T>
	void SetUniform(const std::string& name, const T& value)
	{
		string error = "Specialization of type `" + string(typeid(T).name()) + "` should not be generated. Please implement a manual specialization to resolve this error";
		FATAL_ASSERT(false, error);
	}

	template<>
	void SetUniform<bool>(const std::string& name, const bool& value)
	{
		glUniform1i(GetUniformLocation(name), (int)value);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<int>(const std::string& name, const int& value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}
	template<>
	void SetUniform<uint>(const std::string& name, const uint& value)
	{
		glUniform1ui(GetUniformLocation(name), value);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<float>(const std::string& name, const float& value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<glm::vec2>(const std::string& name, const glm::vec2& value)
	{
		glUniform2fv(GetUniformLocation(name), 1, &value[0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<glm::vec3>(const std::string& name, const glm::vec3& value)
	{
		glUniform3fv(GetUniformLocation(name), 1, &value[0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<glm::vec4>(const std::string& name, const glm::vec4& value)
	{
		glUniform4fv(GetUniformLocation(name), 1, &value[0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<glm::mat2>(const std::string& name, const glm::mat2& mat)
	{
		glUniformMatrix2fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<glm::mat3>(const std::string& name, const glm::mat3& mat)
	{
		glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void SetUniform<glm::mat4>(const std::string& name, const glm::mat4& mat)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	uint GetId() const { return Id; }
	const string& GetName() const { return Name; }

private:

	HashMap<string, int> LocationCache;
	string VertPath;
	string FragPath;
	string GeoPath;
	string ComputePath;
	uint Id;
	string Name;
	enum ShaderType { VERTEX = 0, FRAGMENT, GEOMETRY, COMPUTE,PROGRAM };

	int GetUniformLocation(const string& name)
	{
		if (LocationCache.find(name) != LocationCache.end())
			return LocationCache[name];
		else
		{
			int Location = glGetUniformLocation(Id, name.c_str());

			DYNAMIC_ASSERT(Location != -1, "WARNING: uniform " + name + " does not exist\n");

			LocationCache[name] = Location;
			return Location;
		}
	}
	void CheckCompileErrors(uint shader, ShaderType type)
	{
		int success;
		char infoLog[1024];
		if (type != ShaderType::PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				Array<string, 2> converter = { "VERTEX", "FRAGMENT" };
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR Shader: " << Name << "\n" << "ERROR::SHADER_COMPILATION_ERROR of type: " << converter[type] << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR Shader: " << Name << "\n" << "ERROR::PROGRAM_LINKING_ERROR of type: PROGRAM" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

};