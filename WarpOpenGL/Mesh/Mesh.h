#pragma once
#include <Common/CommonTypes.h>
#include <Common/Profiler.h>
#include <Texture/GLTexture/GLTexture.h>
#include <glad/glad.h>
#include <Shader/Shader.h>
#include <Mesh/Vertex.h>
#include <Buffers/GLBuffers/GLIndexBuffer.h>
#include <Buffers/GLBuffers/GLVertexArray.h>
#include <Renderer/GLRenderer/GLRenderer.h>

class WarpMesh
{
public:

	WarpMesh() = default;

	WarpMesh(Vector<Vertex> Vertices, Vector<uint> Indices, Vector<Ref<GLTexture>> Textures) : 
		Vertices(Vertices)
		, Inidices(Indices)
		, bIsInstanced(false)
		, Textures(Textures)
	{
		PROFILE_FUNCTION();

		for (Ref<GLTexture>& Tex : Textures)
		{
			if (TextureNameCount.find(Tex->Type) != TextureNameCount.end())
			{
				TextureNameCount[Tex->Type]++;
			}
			else
			{
				TextureNameCount[Tex->Type] = 0;
				Textures.push_back(Tex);
			}
		}

		SetupMesh();
	}

	WarpMesh(Vector<Vertex> Vertices, Vector<uint> Indices) : 
		Vertices(Vertices)
		, Inidices(Indices)
	{
		SetupMesh();
	}

	WarpMesh(Vector<Vertex> Vertices, Vector<uint> Indices, Vector<Ref<GLTexParams>> Textures) :
		Vertices(Vertices)
		, Inidices(Indices)
		, TexturesToInit(Textures)
	{

		for (auto& Tex : TexturesToInit)
		{
			if (TextureNameCount.find(Tex->Type) != TextureNameCount.end())
			{
				TextureNameCount[Tex->Type]++;
			}
			else
			{
				TextureNameCount[Tex->Type] = 0;
			}
		}
	}

	WarpMesh(const WarpMesh& Copy)
	{
		Vertices = Copy.Vertices;
		Inidices = Copy.Inidices;
		Textures = Copy.Textures;
		VAO = Copy.VAO;
		VBO = Copy.VBO;
		IBO = Copy.IBO;
		//m_texturesToInit = copy.m_texturesToInit;

		IndexedDraw = Copy.IndexedDraw;
		InstancedDraw = Copy.InstancedDraw;
		bIsInstanced = Copy.bIsInstanced;
		//m_layout = copy.m_layout;
	}

	void SetTextures(Vector<Ref<GLTexture>>& InTextures)
	{
		Textures.insert(Textures.end(), std::make_move_iterator(InTextures.begin()), std::make_move_iterator(InTextures.end()));
	}

	void SetTexture(Ref<GLTexParams> Texture)
	{
		/*m_textures.emplace_back(texture);*/
		Textures.emplace_back(std::make_shared<GLTexture>(Texture));
	}

	void SetTexture(Ref<GLTexture> Texture)
	{
		Textures.push_back(Texture);
	}

	void Draw(GLSLShader& Shader)
	{
		PROFILE_SCOPE("MeshDraw");

		//DYNAMIC_ASSERT(m_textures.size() > 0, "No textures attached to mesh");

		//needs to be an int because it is used to set the shader param and textures are ints
		int TotalTexCount = 0;
		for (auto& Texture : Textures)
		{
			for (uint i = 0; i <= TextureNameCount[Texture->Type]; ++i)
			{
				Shader.SetUniform(Texture->Type + std::to_string(i), TotalTexCount);
				Texture->Bind(TotalTexCount);
				++TotalTexCount;
			}
		}

		if (bIsInstanced)
		{
			g_renderer.DrawInstanced(InstancedDraw);
		}
		else
		{
			g_renderer.DrawIndexed(IndexedDraw);
		}

		//for (Ref<GLTexture>& tex : m_textures)
		//	tex->Unbind();
	}

	void SetVertices(Vector<Vertex>& Vertices)
	{
		Vertices = Vertices;
	}

	void SetIndices(Vector<uint>& Indices)
	{
		Inidices = Indices;
	}

	void InitTextures()
	{
		for (Ref<GLTexParams>& params : TexturesToInit)
		{
			Textures.push_back(std::make_shared<GLTexture>(params));
		}

		TexturesToInit.clear();
	}

	void ClearInitTextures()
	{
		TexturesToInit.clear();
	}

	const Vector<Ref<GLTexParams>>& GetTextureRequests() { return TexturesToInit; }
	Vector<uint> GetIndices() const { return Inidices; }
	Vector<Vertex> GetVertices() const { return Vertices; }

	void SetInstanced(bool bInstanced, uint NumInstances = 0) 
	{ 
		bIsInstanced = bInstanced; 
		if(bInstanced)
		{ 
			InstancedDraw.instanceCount = NumInstances;
			InstancedDraw.IndexBuffer = IBO;
			InstancedDraw.VertexArrayBuffer = VAO;
			InstancedDraw.IndexCount = Inidices.size();
		}
	}
	bool GetIsInstanced() { return bIsInstanced; }

	void SetupMesh()
	{
		PROFILE_FUNCTION();

		FATAL_ASSERT(Inidices.size() > 0 && Vertices.size() > 0, "Attempting to create a mesh with no indices and vertices not set!");
		VAO = std::make_shared<GLVertexArray>();
		VAO->Bind();
		VBO = std::make_shared<GLVertexBuffer>(Vertices.data(), Vertices.size() * sizeof(Vertex));
		IBO = std::make_shared<GLIndexBuffer>(Inidices.data(), Inidices.size());

		GLVertexBufferLayout layout;
		layout.Push<Vertex>(1);

		VAO->AddBuffer(*VBO, layout);

		//setup Draws. Kinda wasteful but whatever

		InstancedDraw.IndexBuffer = IBO;
		InstancedDraw.VertexArrayBuffer = VAO;
		InstancedDraw.IndexCount = Inidices.size();

		IndexedDraw.PrimitiveCount = Inidices.size();
		IndexedDraw.VertexArrayBuffer = VAO;
		IndexedDraw.IndexBuffer = IBO;

		
	}

	const GLIndexedDrawCall& GetIndexedDrawCall() const { return IndexedDraw; }

	Vector<Ref<GLTexture>> GetTextures()
	{
		return Textures;
	}

	Ref<GLTexture> GetTexture(uint index)
	{
		return Textures[index];
	}

private:
	//cpu copies
	Vector<Vertex> Vertices;
	Vector<uint> Inidices;
	Vector<Ref<GLTexture>> Textures;
	Vector<Ref<GLTexParams>> TexturesToInit;
	bool bIsInstanced = false;
	//incase of multiple textures
	HashMap<string, uint> TextureNameCount;
	
	GLIndexedDrawCall IndexedDraw;
	GLInstancedDrawCall InstancedDraw;

	Ref<GLVertexArray> VAO;
	Ref<GLVertexBuffer> VBO;
	Ref<GLIndexBuffer> IBO;

};

