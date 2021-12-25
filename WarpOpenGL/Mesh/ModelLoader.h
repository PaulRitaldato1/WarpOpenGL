#pragma once
#include <Common/GPUMarker.h>
#include <Common/Profiler.h>
#include <Common/Assert.h>
#include <Common/CommonTypes.h>

#include <Texture/GLTexture/GLTexture.h>
#include <Mesh/Model.h>

#include <Parallelization/Threading/ThreadPool.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <fstream>
#include <sstream>

//TODO should this be a singleton? 
// 
//separating out the loader from the model. This allows global texture tracking and easier threading
class ModelLoader
{
public:
	ModelLoader(string resourceDirectory)
		: m_resourceDirectory(resourceDirectory)
		, m_pool(4)
	{}

	Vector<Ref<Model>> loadModelsAsync(Vector<ModelDesc>& modelArgs);
	Ref<Model> loadModel(ModelDesc& desc);

	Ref<Model> generateQuad();

	Ref<Model> generateGrid(int width, int depth, int m, int n, glm::vec3 pos);

	Ref<Model> generateSphereInstanced(Vector<float>& radii, Vector<glm::mat4>& transforms, uint sliceCount, uint stackCount, Ref<GLTexParams> tex = nullptr);

	Ref<Model> generateSphere(float radius, uint sliceCount, uint stackCount, glm::vec3 pos, Ref<GLTexParams> tex = nullptr);
	Vector<Ref<GLTexture>> getTextures()
	{
		Vector<Ref<GLTexture>> textures;
		for (auto& [name, texture] : m_textures)
		{
			textures.push_back(texture);
		}

		return textures;
	}

private:

	void traverseSceneGraph(aiNode* node, string& directory, Vector<Ref<Mesh>>& meshList, const aiScene* scene);

	Ref<Mesh> loadMesh(aiMesh* mesh, string& directory, const aiScene* scene);

	Vector<Ref<GLTexParams>> loadTextures(aiMaterial* mat, aiTextureType type, string typeName, string& directory);

	//maybe use a uint hash instead of string, hash func would need to be faster than std::hash
	HashMap<string, Ref<GLTexParams>> m_loadedTextures;
	HashMap<string, Ref<GLTexture>> m_textures;
	HashMap<string, Ref<Model>> m_modelsLoaded;

	string m_resourceDirectory;

	ThreadPool m_pool;
};