#include <Mesh/ModelLoader.h>
#include <Mesh/GeoGen.h>

Ref<Model> ModelLoader::GenerateQuad()
{
	Vector<Ref<WarpMesh>> Meshes;
	Meshes.push_back(GeoGen::CreateDefaultQuad());

	ModelDesc Desc;
	Desc.bIsInstanced = false;
	Desc.bGamma = false;
	Desc.bHasShadow = false;
	Desc.Transform = glm::mat4(1.0f);

	return std::make_shared<Model>(Meshes, Desc);
}

Ref<Model> ModelLoader::GenerateGrid(int Width, int Depth, int M, int N, glm::vec3 Pos)
{
	Vector<Ref<WarpMesh>> Meshes;
	Meshes.push_back(std::make_shared<WarpMesh>(GeoGen::CreateGrid(Width, Depth, M, N)));

	ModelDesc Desc;
	Desc.bIsInstanced = false;
	Desc.bGamma = false;
	Desc.bHasShadow = false;

	glm::mat4 ModelMatrix(1.0f);
	Desc.Transform = glm::translate(ModelMatrix, Pos);

	return std::make_shared<Model>(Meshes, Desc);
}

Ref<Model> ModelLoader::GenerateSphereInstanced(Vector<float>& Radii, Vector<glm::mat4>& Transforms, uint SliceCount, uint StackCount, Ref<GLTexParams> TexParams /*= nullptr*/)
{

	DYNAMIC_ASSERT(Radii.size() == Transforms.size(), "These need to be the same size");

	Vector<Ref<WarpMesh>> Meshes;
	Meshes.push_back(GeoGen::CreateSphere(1.0f, SliceCount, StackCount));

	for (uint i = 0; i < Radii.size(); i++)
	{
		float Radius = Radii[i];
		Transforms[i] = glm::scale(Transforms[i], glm::vec3(Radius, Radius, Radius));
	}
	

	ModelDesc Desc;
	Desc.bIsInstanced = true;
	Desc.bGamma = false;
	Desc.bHasShadow = false;
	Desc.Instances = Transforms;

	if (TexParams != nullptr)
		Meshes.back()->SetTexture(TexParams);

	Ref<Model> RetModel = std::make_shared<Model>(Meshes, Desc);
	//RetModel->GenBuffers();
	return RetModel;
}

Ref<Model> ModelLoader::GenerateSphere(float Radius, uint SliceCount, uint StackCount, glm::vec3 Pos, Ref<GLTexParams> TexParams)
{
	Vector<Ref<WarpMesh>> Meshes;
	Meshes.push_back(GeoGen::CreateSphere(Radius, SliceCount, StackCount));

	ModelDesc Desc;
	Desc.bIsInstanced = false;
	Desc.bGamma = false;
	Desc.bHasShadow = false;

	glm::mat4 ModelMatrix(1.0f);
	ModelMatrix[3] = glm::vec4(Pos, 1.0f);
	Desc.Transform = ModelMatrix;

	if(TexParams != nullptr)
		Meshes.back()->SetTexture(TexParams);

	return std::make_shared<Model>(Meshes, Desc);
}

Vector<Ref<Model>> ModelLoader::LoadModelsAsync(Vector<ModelDesc>& modelArgs, uint numThreads)
{
	PROFILE_SCOPE("ModelLoader::LoadModelAsync");
	
	ThreadPool Pool(numThreads);

	stbi_set_flip_vertically_on_load(true);

	Vector<std::future<Ref<Model>>> Futures;
	auto LoadFunc = [this](ModelDesc& args) { return LoadModel(args); };
	for (auto& args : modelArgs)
	{
		Futures.push_back(Pool.enqueue(LoadFunc, args));
	}

	Vector<Ref<Model>> Models;
	for (auto& FutureItem : Futures)
	{
		Models.push_back(FutureItem.get());
	}

	//gl calls need to be in the same thread
	for (auto& [path, params] : LoadedTextures)
	{
		Textures[path] = std::make_shared<GLTexture>(params);
	}

	for (auto& ModelItem : Models)
	{
		ModelItem->GenBuffers();
		for (const auto& MeshItem : ModelItem->GetMeshes())
		{
			//gl calls to create buffers. Needs to be in main thread
			MeshItem->SetupMesh();
			Vector<Ref<GLTexture>> TexturesToGive;
			const auto& TexRequests = MeshItem->GetTextureRequests();
			for (const auto& Tex : TexRequests)
			{
				TexturesToGive.push_back(Textures[Tex->Path]);
			}
			MeshItem->SetTextures(TexturesToGive);
			MeshItem->ClearInitTextures();
		}
	}
	return Models;
}

Ref<Model> ModelLoader::LoadModel(ModelDesc& Desc)
{
	PROFILE_FUNCTION();
	Assimp::Importer Importer;
	const aiScene* Scene = Importer.ReadFile(Desc.Path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!Scene || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !Scene->mRootNode)
	{
		FATAL_ASSERT(false, "ERROR::ASSIMP:: " << Importer.GetErrorString() << std::endl);
		return nullptr;
	}
	
	Vector<Ref<WarpMesh>> meshList;
	string directory = Desc.Path.substr(0, Desc.Path.find_last_of('/'));
	TraverseSceneGraph(Scene->mRootNode, directory, meshList, Scene);
	return std::make_shared<Model>(meshList, Desc);
}

void ModelLoader::TraverseSceneGraph(aiNode* Node, string& Directory, Vector<Ref<WarpMesh>>& MeshList, const aiScene* Scene)
{
	PROFILE_FUNCTION();

	for (uint i = 0; i < Node->mNumMeshes; ++i)
	{
		aiMesh* mesh = Scene->mMeshes[Node->mMeshes[i]];
		MeshList.push_back(LoadMesh(mesh, Directory, Scene));
	}

	for (uint i = 0; i < Node->mNumChildren; ++i)
	{
		TraverseSceneGraph(Node->mChildren[i], Directory, MeshList, Scene);
	}
}

Ref<WarpMesh> ModelLoader::LoadMesh(aiMesh* Mesh, string& Directory, const aiScene* Scene)
{
	PROFILE_FUNCTION();

	Vector<Vertex> Vertices;
	Vector<uint> Indices;
	Vector<Ref<GLTexParams>> Textures;

	for (uint i = 0; i < Mesh->mNumVertices; ++i)
	{
		Vertex CurrentVertex;
		glm::vec3 ReusableVector;

		//positions
		ReusableVector.x = Mesh->mVertices[i].x;
		ReusableVector.y = Mesh->mVertices[i].y;
		ReusableVector.z = Mesh->mVertices[i].z;
		CurrentVertex.Position = ReusableVector;

		//normals
		if (Mesh->HasNormals())
		{
			ReusableVector.x = Mesh->mNormals[i].x;
			ReusableVector.y = Mesh->mNormals[i].y;
			ReusableVector.z = Mesh->mNormals[i].z;
			CurrentVertex.Normal = ReusableVector;
		}

		//tex coords
		if (Mesh->mTextureCoords[0])
		{
			glm::vec2 Vec;

			Vec.x = Mesh->mTextureCoords[0][i].x;
			Vec.y = Mesh->mTextureCoords[0][i].y;
			CurrentVertex.TexCoords = Vec;

			//tangent
			ReusableVector.x = Mesh->mTangents[i].x;
			ReusableVector.y = Mesh->mTangents[i].y;
			ReusableVector.z = Mesh->mTangents[i].z;
			CurrentVertex.Tangent = ReusableVector;

			//bitangent
			ReusableVector.x = Mesh->mBitangents[i].x;
			ReusableVector.y = Mesh->mBitangents[i].y;
			ReusableVector.z = Mesh->mBitangents[i].z;
			CurrentVertex.Bitangent = ReusableVector;
		}
		else
		{
			CurrentVertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		Vertices.push_back(CurrentVertex);

	}

	for (uint i = 0; i < Mesh->mNumFaces; ++i)
	{
		aiFace face = Mesh->mFaces[i];
		for (uint j = 0; j < face.mNumIndices; ++j)
		{
			Indices.push_back(face.mIndices[j]);
		}
	}

	aiMaterial* Material = Scene->mMaterials[Mesh->mMaterialIndex];

	//1 diffuse maps
	Vector<Ref<GLTexParams>> DiffuseMaps = LoadTextures(Material, aiTextureType_DIFFUSE, "texture_diffuse", Directory);
	Textures.insert(Textures.end(), DiffuseMaps.begin(), DiffuseMaps.end());
	//2 spec maps
	Vector<Ref<GLTexParams>> SpecularMaps = LoadTextures(Material, aiTextureType_SPECULAR, "texture_specular", Directory);
	Textures.insert(Textures.end(), SpecularMaps.begin(), SpecularMaps.end());
	//3 normal maps
	Vector<Ref<GLTexParams>> NormalMaps = LoadTextures(Material, aiTextureType_HEIGHT, "texture_normal", Directory);
	Textures.insert(Textures.end(), NormalMaps.begin(), NormalMaps.end());
	//4 height maps
	Vector<Ref<GLTexParams>> HeightMaps = LoadTextures(Material, aiTextureType_AMBIENT, "texture_height", Directory);
	Textures.insert(Textures.end(), HeightMaps.begin(), HeightMaps.end());

	return std::make_shared<WarpMesh>(Vertices, Indices, Textures);
}

Vector<Ref<GLTexParams>> ModelLoader::LoadTextures(aiMaterial* Mat, aiTextureType Type, string TypeName, string& Directory)
{
	PROFILE_FUNCTION();

	Vector<Ref<GLTexParams>> Textures;
	for (uint i = 0; i < Mat->GetTextureCount(Type); ++i)
	{
		aiString Str;
		Mat->GetTexture(Type, i, &Str);

		string fullPath = Directory + "/" + string(Str.C_Str());
		if (LoadedTextures.find(fullPath) == LoadedTextures.end())
		{
			Ref<GLTexParams> params = LoadGLTexture(Str.C_Str(), Directory.c_str(), TypeName);

			//GLTexture texture(params);
			//Ref<GLTexture> texture = makeRef<GLTexture>(params); //this limits to 1 thread. GLcalls need to be in 1 thrad
			LoadedTextures[fullPath] = params;
			Textures.push_back(params);
		}
		else
		{
			Textures.push_back(LoadedTextures[fullPath]);
		}
	}
	return Textures;
}

