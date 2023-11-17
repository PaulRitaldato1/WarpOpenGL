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
		: Path(path)
		, bGamma(gamma)
		, bHasShadow(hasShadow)
		, bIsInstanced(false)
		, Transform(transform)
	{}

	ModelDesc(string path, bool gamma, bool hasShadow, Vector<glm::mat4> instances)
		: Path(path)
		, bGamma(gamma)
		, bHasShadow(hasShadow)
		, bIsInstanced(true)
		, Instances(instances)
	{}

	ModelDesc() = default;

	string Path;
	bool bGamma;
	bool bHasShadow;
	bool bIsInstanced;
	Vector<glm::mat4> Instances;
	glm::mat4 Transform;
};

class Model
{
public:

	Model(Vector<Ref<WarpMesh>>& MeshList, ModelDesc& Desc) :
		Meshes(std::move(MeshList))
		, bIsInstanced(Desc.bIsInstanced)
		, bGammaCorrection(Desc.bGamma)
		, bHasShadow(Desc.bHasShadow)
		, Path(Desc.Path)
	{
		Name = Path.substr(Path.find_last_of('/') + 1, Path.size()-1);
		Name = Name.substr(0, Name.find_last_of('.'));

		for (auto& MeshItem : Meshes)
			MeshItem->SetInstanced(bIsInstanced, Desc.Instances.size());

		if (bIsInstanced)
		{
			// constructor is called in another thread, these glcalls will "fail"
		//	m_ssbo = std::make_unique<GLSSBO>(desc.instances.size() * sizeof(glm::mat4), 1);
			Instances.insert(Instances.end(), std::make_move_iterator(Desc.Instances.begin()), std::make_move_iterator(Desc.Instances.end()));
		}
		else
			SetTransform(Desc.Transform);

		Material = g_MaterialPresets[MaterialPresets::DEFAULT];
	}

	Model(Ref<WarpMesh> mesh)
	{
		bIsInstanced = false;
		Meshes.push_back(mesh);
	}

	Model(const Model& copy) = delete;

	void InitTextures()
	{
		for (Ref<WarpMesh>& mesh : Meshes)
		{
			mesh->InitTextures();
		}
	}

	void Draw(GLSLShader& Shader) const
	{
		string s = Name + " draw";
		PROFILE_SCOPE(s.c_str());
		GPUMarker(s);


		Shader.Bind();
		
		//shader.setUniform("mat.GlobalAmbient", m_material.GlobalAmbient);
		//shader.setUniform("mat.AmbientColor", m_material.AmbientColor);

		//shader.setUniform("mat.EmissiveColor", m_material.EmissiveColor);
		Shader.SetUniform("mat.DiffuseColor", Material.DiffuseColor);
		//shader.setUniform("mat.SpecularColor", m_material.SpecularColor);

		//shader.setUniform("mat.Reflectance", m_material.Reflectance);

		//shader.setUniform("mat.Opacity", m_material.Opacity);
		//shader.setUniform("mat.SpecularPower", m_material.SpecularPower);
		//shader.setUniform("mat.IndexOfRefraction", m_material.IndexOfRefraction);

		//shader.setUniform("mat.HasAmbientTexture", m_material.HasAmbientTexture);
		//shader.setUniform("mat.HasEmissiveTexture", m_material.HasEmissiveTexture);
		Shader.SetUniform("mat.HasDiffuseTexture", Material.bHasDiffuseTexture);
		Shader.SetUniform("mat.HasSpecularTexture", Material.bHasSpecularTexture);
		//shader.setUniform("mat.HasSpecularPowerTexture", m_material.HasSpecularPowerTexture);
		//shader.setUniform("mat.HasNormalTexture", m_material.HasNormalTexture);

		//shader.setUniform("mat.SpecularScale", m_material.SpecularScale);
		//shader.setUniform("mat.AlphaThreshold", m_material.AlphaThreshold);
		Shader.SetUniform("mat.shininess", Material.Shininess);

		if (bIsInstanced && SSBO->IsDirty())
		{
			SSBO->BindSubdata<Vector<glm::mat4>> (Instances);
		}
		else if(!bIsInstanced)
		{
			Shader.SetUniform("model", m_modelTransform);
		}

		for (auto& MeshItem : Meshes)
			MeshItem->Draw(Shader);
	}


	glm::mat4 GetTransform() const { return m_modelTransform; }
	void SetTransform(glm::mat4 Transform) { m_modelTransform = Transform; }

	glm::vec3 GetPosition() const { return glm::vec3(m_modelTransform[3]); }

	const Vector<Ref<WarpMesh>>& GetMeshes() const { return Meshes; }

	void SetInstances(Vector<glm::mat4>& instances)
	{
		PROFILE_FUNCTION();
		Instances.clear();
		Instances.insert(Instances.end(), std::make_move_iterator(instances.begin()), std::make_move_iterator(instances.end()));

		SSBO = std::make_unique<GLSSBO>(instances.size() * sizeof(glm::mat4), 1);

		for (auto& MeshItem : Meshes)
		{
			MeshItem->SetInstanced(true, Instances.size());
		}
	}

	Vector<glm::mat4> GetInstances() { return Instances; }

	void GenBuffers()
	{
		if(bIsInstanced)
			SSBO = std::make_unique<GLSSBO>(Instances.size() * sizeof(glm::mat4), 1);
	}

	bool GetIsInstanced() const { return bIsInstanced; }

	bool GetHasShadow() const { return bHasShadow; }
	bool SetHasShadow(bool value) { bHasShadow = value; }

	Vector<Ref<GLTexture>> GetTexturesFromAMesh()
	{
		return Meshes.back()->GetTextures();
	}

private:

	Vector<Ref<WarpMesh>> Meshes;
	string Path;
	string Name;

	glm::mat4 m_modelTransform;

	bool bGammaCorrection;
	bool bHasShadow;
	bool bIsInstanced;
	URef<GLSSBO> SSBO; //ssbo for instanced models
	Vector<glm::mat4> Instances;
	Material Material;
};