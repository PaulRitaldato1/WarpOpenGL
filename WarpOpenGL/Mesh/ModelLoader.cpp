#include <Mesh/ModelLoader.h>

Vector<Ref<Model>> ModelLoader::loadModelsAsync(Vector<ModelDesc>& modelArgs)
{
	PROFILE_SCOPE("ModelLoader::LoadModelAsync");
	
	stbi_set_flip_vertically_on_load(true);

	Vector<std::future<Ref<Model>>> futures;
	auto loadFunc = [this](ModelDesc& args) { return loadModel(args); };
	for (auto& args : modelArgs)
	{
		futures.push_back(m_pool.enqueue(loadFunc, args));
	}

	Vector<Ref<Model>> models;
	for (auto& fut : futures)
	{
		models.push_back(fut.get());
	}

	//gl calls need to be in the same thread
	for (auto& [path, params] : m_loadedTextures)
	{
		m_textures[path] = std::make_shared<GLTexture>(params);
	}

	for (auto& model : models)
	{
		model->genBuffers();
		for (const auto& mesh : model->getMeshes())
		{
			//gl calls to create buffers. Needs to be in main thread
			mesh->setupMesh();
			Vector<Ref<GLTexture>> texturesToGive;
			const auto& texRequests = mesh->getTextureRequests();
			for (const auto& tex : texRequests)
			{
				texturesToGive.push_back(m_textures[tex->Path]);
			}
			mesh->setTextures(texturesToGive);
			mesh->clearInitTextures();
		}
	}
	return models;
}

Ref<Model> ModelLoader::loadModel(ModelDesc& desc)
{
	PROFILE_FUNCTION();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(desc.path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		FATAL_ASSERT(false, "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl);
		return nullptr;
	}
	
	Vector<Ref<Mesh>> meshList;
	string directory = desc.path.substr(0, desc.path.find_last_of('/'));
	traverseSceneGraph(scene->mRootNode, directory, meshList, scene);
	return std::make_shared<Model>(meshList, desc);
}

void ModelLoader::traverseSceneGraph(aiNode* node, string& directory, Vector<Ref<Mesh>>& meshList, const aiScene* scene)
{
	PROFILE_FUNCTION();

	for (uint i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshList.push_back(loadMesh(mesh, directory, scene));
	}

	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		traverseSceneGraph(node->mChildren[i], directory, meshList, scene);
	}
}

Ref<Mesh> ModelLoader::loadMesh(aiMesh* mesh, string& directory, const aiScene* scene)
{
	PROFILE_FUNCTION();

	Vector<Vertex> vertices;
	Vector<uint> indices;
	Vector<Ref<GLTexParams>> textures;

	for (uint i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		glm::vec3 vector;

		//positions
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		//normals
		if (mesh->HasNormals())
		{
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;
		}

		//tex coords
		if (mesh->mTextureCoords[0])
		{
			glm::vec2 vec;

			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			//tangent
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = vector;

			//bitangent
			vector.x = mesh->mBitangents[i].x;
			vector.y = mesh->mBitangents[i].y;
			vector.z = mesh->mBitangents[i].z;
			vertex.Bitangent = vector;
		}
		else
		{
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);

	}

	for (uint i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	//1 diffuse maps
	Vector<Ref<GLTexParams>> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//2 spec maps
	Vector<Ref<GLTexParams>> specularMaps = loadTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	//3 normal maps
	Vector<Ref<GLTexParams>> normalMaps = loadTextures(material, aiTextureType_NORMALS, "texture_normal", directory);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	//4 height maps
	Vector<Ref<GLTexParams>> heightMaps = loadTextures(material, aiTextureType_HEIGHT, "texture_height", directory);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	return std::make_shared<Mesh>(vertices, indices, textures);
}

Vector<Ref<GLTexParams>> ModelLoader::loadTextures(aiMaterial* mat, aiTextureType type, string typeName, string& directory)
{
	PROFILE_FUNCTION();

	Vector<Ref<GLTexParams>> textures;
	for (uint i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		string fullPath = directory + "/" + string(str.C_Str());
		if (m_loadedTextures.find(fullPath) == m_loadedTextures.end())
		{
			Ref<GLTexParams> params = loadGLTexture(str.C_Str(), directory.c_str(), typeName);

			//GLTexture texture(params);
			//Ref<GLTexture> texture = makeRef<GLTexture>(params); //this limits to 1 thread. GLcalls need to be in 1 thrad
			m_loadedTextures[fullPath] = params;
			textures.push_back(params);
		}
		else
		{
			textures.push_back(m_loadedTextures[fullPath]);
		}
	}
	return textures;
}

