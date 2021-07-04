#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Texture/stb_image.h>
#include <Texture/GLTexture/GLTexture.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Mesh/Mesh.h>
#include <Shader/Shader.h>

#include <Common/CommonTypes.h>

#include <fstream>
#include <sstream>

#include <Parallelization/Threading/ThreadPool.h>
#include <Common/GPUMarker.h>
#include <Common/Profiler.h>

#include <Buffers/GLBuffers/GLSSBO.h>
#include <Renderer/GLRenderer/GLRenderer.h>

struct ModelDesc
{
	string path;
	bool gamma;
	bool isInstanced;
	Vector<glm::mat4> instances;
};

class Model
{
public:

	Model(Vector<Ref<Mesh>>& meshList, ModelDesc& desc) :
		m_meshes(std::move(meshList))
		, m_isInstanced(desc.isInstanced)
		, m_gammaCorrection(desc.gamma)
		, m_path(desc.path)
	{
		m_name = m_path.substr(m_path.find_last_of('/') + 1, m_path.size()-1);
		m_name = m_name.substr(0, m_name.find_last_of('.'));

		for (auto& mesh : m_meshes)
			mesh->setInstanced(m_isInstanced, desc.instances.size());

		if (m_isInstanced)
		{
			// constructor is called in another thread, these glcalls will "fail"
		//	m_ssbo = std::make_unique<GLSSBO>(desc.instances.size() * sizeof(glm::mat4), 1);
			m_instances.insert(m_instances.end(), std::make_move_iterator(desc.instances.begin()), std::make_move_iterator(desc.instances.end()));
		}
	}

	Model(const Model& copy) = delete;

	void initTextures()
	{
		for (Ref<Mesh>& mesh : m_meshes)
		{
			mesh->initTextures();
		}
	}

	void Draw(Shader& shader)
	{
		PROFILE_FUNCTION();
		Marker marker(m_name);

		if (m_isInstanced)
		{
			m_ssbo->BindSubdata<Vector<glm::mat4>> (m_instances);
		}

		shader.Bind();
		shader.setUniform("model", m_modelTransform);
		for (auto& mesh : m_meshes)
			mesh->Draw(shader);
	}


	const glm::mat4& getTransform() const { return m_modelTransform; }
	void setTransform(glm::mat4 transform) { m_modelTransform = transform; }

	const Vector<Ref<Mesh>>& getMeshes() const { return m_meshes; }

	void updateInstances(Vector<glm::mat4>& instances)
	{
		PROFILE_FUNCTION();
		m_instances.clear();
		m_instances.insert(m_instances.end(), std::make_move_iterator(instances.begin()), std::make_move_iterator(instances.end()));

		m_ssbo = std::make_unique<GLSSBO>(instances.size() * sizeof(glm::mat4), 1);
	}

	void genBuffers()
	{
		if(m_isInstanced)
			m_ssbo = std::make_unique<GLSSBO>(m_instances.size() * sizeof(glm::mat4), 1);
	}

	bool getIsInstanced() const { return m_isInstanced; }
private:

	Vector<Ref<Mesh>> m_meshes;
	string m_path;
	string m_name;

	//unused for now
	glm::mat4 m_modelTransform;

	bool m_gammaCorrection;
	
	bool m_isInstanced;
	URef<GLSSBO> m_ssbo; //ssbo for instanced models
	Vector<glm::mat4> m_instances;
};