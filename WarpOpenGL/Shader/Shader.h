#pragma once
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <Common/CommonTypes.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Common/Assert.h>

class Shader
{
public:


	Shader(string vertexPath, string fragmentPath, string geometryPath = "") :
		m_vertPath(vertexPath)
		, m_fragPath(fragmentPath)
		, m_geoPath(geometryPath)
	{
		compile();
	}

	//Shader(string vertexPath, string fragmentPath, string geometryPath = "") :
	//	m_vertPath(vertexPath)
	//	, m_fragPath(fragmentPath)
	//	, m_geoPath(geometryPath)
	//{
	//	compile();
	//}
	Shader(string computePath) : m_computePath(computePath)
	{
		string cCode;

		std::ifstream cFile;

		cFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			cFile.open(m_computePath);
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
		checkCompileErrors(computeId, shaderType::COMPUTE);

		m_id = glCreateProgram();
		glAttachShader(m_id, computeId);
		glLinkProgram(m_id);
		checkCompileErrors(m_id, shaderType::PROGRAM);

		glDeleteShader(computeId);
	}

	//separate gl calls into their own function
	void compile()
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
			vFile.open(m_vertPath);
			fFile.open(m_fragPath);

			std::stringstream vStream;
			std::stringstream fStream;
			vStream << vFile.rdbuf();
			fStream << fFile.rdbuf();

			vFile.close();
			fFile.close();

			vCode = vStream.str();
			fCode = fStream.str();

			if (m_geoPath != "")
			{
				gFile.open(m_geoPath);
				std::stringstream gStream;
				gStream << gFile.rdbuf();
				gFile.close();
				gCode = gStream.str();
			}

		}
		catch (std::ifstream::failure& e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
		}

		const char* vShaderCode = vCode.c_str();
		const char* fShaderCode = fCode.c_str();

		uint vertId = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertId, 1, &vShaderCode, NULL);
		glCompileShader(vertId);
		checkCompileErrors(vertId, shaderType::VERTEX);

		uint fragId = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragId, 1, &fShaderCode, NULL);
		glCompileShader(fragId);
		checkCompileErrors(fragId, shaderType::FRAGMENT);

		uint geoId;
		if (m_geoPath != "")
		{
			const char* gShaderCode = gCode.c_str();
			geoId = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geoId, 1, &gShaderCode, NULL);
			glCompileShader(geoId);
			checkCompileErrors(geoId, shaderType::GEOMETRY);
		}

		m_id = glCreateProgram();
		glAttachShader(m_id, vertId);
		glAttachShader(m_id, fragId);
		if (m_geoPath != "")
			glAttachShader(m_id, geoId);
		glLinkProgram(m_id);
		checkCompileErrors(m_id, shaderType::PROGRAM);

		glDeleteShader(vertId);
		glDeleteShader(fragId);
		if (m_geoPath != "")
			glDeleteShader(geoId);

	}

	void Bind()
	{
		glUseProgram(m_id);
	}

	void Unbind()
	{
		glUseProgram(0);
	}
	template<typename T>
	void setUniform(const std::string& name, const T& value)
	{
		string error = "Specialization of type `" + string(typeid(T).name()) + "` should not be generated. Please implement a manual specialization to resolve this error";
		FATAL_ASSERT(false, error);
	}

	template<>
	void setUniform<bool>(const std::string& name, const bool& value)
	{
		glUniform1i(getUniformLocation(name), (int)value);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<int>(const std::string& name, const int& value)
	{
		glUniform1i(getUniformLocation(name), value);
	}
	template<>
	void setUniform<uint>(const std::string& name, const uint& value)
	{
		glUniform1i(getUniformLocation(name), value);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<float>(const std::string& name, const float& value)
	{
		glUniform1f(getUniformLocation(name), value);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<glm::vec2>(const std::string& name, const glm::vec2& value)
	{
		glUniform2fv(getUniformLocation(name), 1, &value[0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<glm::vec3>(const std::string& name, const glm::vec3& value)
	{
		glUniform3fv(getUniformLocation(name), 1, &value[0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<glm::vec4>(const std::string& name, const glm::vec4& value)
	{
		glUniform4fv(getUniformLocation(name), 1, &value[0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<glm::mat2>(const std::string& name, const glm::mat2& mat)
	{
		glUniformMatrix2fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<glm::mat3>(const std::string& name, const glm::mat3& mat)
	{
		glUniformMatrix3fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}
	// ------------------------------------------------------------------------
	template<>
	void setUniform<glm::mat4>(const std::string& name, const glm::mat4& mat)
	{
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &mat[0][0]);
	}

	uint getId() const { return m_id; }
private:

	HashMap<string, int> m_locationCache;
	string m_vertPath;
	string m_fragPath;
	string m_geoPath;
	string m_computePath;
	uint m_id;

	enum shaderType { VERTEX = 0, FRAGMENT, GEOMETRY, COMPUTE,PROGRAM };

	int getUniformLocation(const string& name)
	{
		if (m_locationCache.find(name) != m_locationCache.end())
			return m_locationCache[name];
		else
		{
			int location = glGetUniformLocation(m_id, name.c_str());

			DYNAMIC_ASSERT(location != -1, "WARNING: uniform " + name + " does not exist\n");

			m_locationCache[name] = location;
			return location;
		}
	}
	void checkCompileErrors(uint shader, shaderType type)
	{
		int success;
		char infoLog[1024];
		if (type != shaderType::PROGRAM)
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				Array<string, 2> converter = { "VERTEX", "FRAGMENT" };
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << converter[type] << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: PROGRAM" << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}
		}
	}

};