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

#include <Lighting/Material.h>

struct ModelDesc
{
	ModelDesc(string path, bool gamma, bool hasShadow, glm::mat4 transform)
		: path(path)
		, gamma(gamma)
		, hasShadow(hasShadow)
		, isInstanced(false)
		, transform(transform)
	{}

	ModelDesc(string path, bool gamma, bool hasShadow, Vector<glm::mat4> instances)
		: path(path)
		, gamma(gamma)
		, hasShadow(hasShadow)
		, isInstanced(true)
		, instances(instances)
	{}

	ModelDesc() = default;

	string path;
	bool gamma;
	bool hasShadow;
	bool isInstanced;
	Vector<glm::mat4> instances;
	glm::mat4 transform;
};

class Model
{
public:

	Model(Vector<Ref<Mesh>>& meshList, ModelDesc& desc) :
		m_meshes(std::move(meshList))
		, m_isInstanced(desc.isInstanced)
		, m_gammaCorrection(desc.gamma)
		, m_hasShadow(desc.hasShadow)
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
		else
			setTransform(desc.transform);

		m_material = g_MaterialPresets[MaterialPresets::DEFAULT];
	}

	Model(Ref<Mesh> mesh)
	{
		m_isInstanced = false;
		m_meshes.push_back(mesh);
	}

	Model(const Model& copy) = delete;

	void initTextures()
	{
		for (Ref<Mesh>& mesh : m_meshes)
		{
			mesh->initTextures();
		}
	}

	void Draw(Shader& shader) const
	{
		string s = m_name + " draw";
		PROFILE_SCOPE(s.c_str());
		GPUMarker(s);


		shader.Bind();
		
		//shader.setUniform("mat.GlobalAmbient", m_material.GlobalAmbient);
		//shader.setUniform("mat.AmbientColor", m_material.AmbientColor);

		//shader.setUniform("mat.EmissiveColor", m_material.EmissiveColor);
		shader.setUniform("mat.DiffuseColor", m_material.DiffuseColor);
		//shader.setUniform("mat.SpecularColor", m_material.SpecularColor);

		//shader.setUniform("mat.Reflectance", m_material.Reflectance);

		//shader.setUniform("mat.Opacity", m_material.Opacity);
		//shader.setUniform("mat.SpecularPower", m_material.SpecularPower);
		//shader.setUniform("mat.IndexOfRefraction", m_material.IndexOfRefraction);

		//shader.setUniform("mat.HasAmbientTexture", m_material.HasAmbientTexture);
		//shader.setUniform("mat.HasEmissiveTexture", m_material.HasEmissiveTexture);
		shader.setUniform("mat.HasDiffuseTexture", m_material.HasDiffuseTexture);
		shader.setUniform("mat.HasSpecularTexture", m_material.HasSpecularTexture);
		//shader.setUniform("mat.HasSpecularPowerTexture", m_material.HasSpecularPowerTexture);
		//shader.setUniform("mat.HasNormalTexture", m_material.HasNormalTexture);

		//shader.setUniform("mat.SpecularScale", m_material.SpecularScale);
		//shader.setUniform("mat.AlphaThreshold", m_material.AlphaThreshold);
		shader.setUniform("mat.shininess", m_material.shininess);

		if (m_isInstanced && m_ssbo->isDirty())
		{
			m_ssbo->BindSubdata<Vector<glm::mat4>> (m_instances);
		}
		else if(!m_isInstanced)
		{
			shader.setUniform("model", m_modelTransform);
		}

		for (auto& mesh : m_meshes)
			mesh->Draw(shader);
	}


	glm::mat4 getTransform() const { return m_modelTransform; }
	void setTransform(glm::mat4 transform) { m_modelTransform = transform; }

	glm::vec3 getPosition() const { return glm::vec3(m_modelTransform[3]); }

	const Vector<Ref<Mesh>>& getMeshes() const { return m_meshes; }

	void setInstances(Vector<glm::mat4>& instances)
	{
		PROFILE_FUNCTION();
		m_instances.clear();
		m_instances.insert(m_instances.end(), std::make_move_iterator(instances.begin()), std::make_move_iterator(instances.end()));

		m_ssbo = std::make_unique<GLSSBO>(instances.size() * sizeof(glm::mat4), 1);

		for (auto& mesh : m_meshes)
		{
			mesh->setInstanced(true, m_instances.size());
		}
	}

	Vector<glm::mat4> getInstances() { return m_instances; }

	void genBuffers()
	{
		if(m_isInstanced)
			m_ssbo = std::make_unique<GLSSBO>(m_instances.size() * sizeof(glm::mat4), 1);
	}

	bool getIsInstanced() const { return m_isInstanced; }

	bool getHasShadow() const { return m_hasShadow; }
	bool setHasShadow(bool value) { m_hasShadow = value; }

	Vector<Ref<GLTexture>> getTexturesFromAMesh()
	{
		return m_meshes.back()->getTextures();
	}

private:

	Vector<Ref<Mesh>> m_meshes;
	string m_path;
	string m_name;

	glm::mat4 m_modelTransform;

	bool m_gammaCorrection;
	bool m_hasShadow;
	bool m_isInstanced;
	URef<GLSSBO> m_ssbo; //ssbo for instanced models
	Vector<glm::mat4> m_instances;
	Material m_material;
};