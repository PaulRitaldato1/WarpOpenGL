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

class Mesh
{
public:

	Mesh() = default;

	Mesh(Vector<Vertex> vertices, Vector<uint> indices, Vector<Ref<GLTexture>> textures) : 
		m_vertices(vertices)
		, m_indices(indices)
		, m_isInstanced(false)
		, m_textures(textures)
	{
		PROFILE_FUNCTION();

		for (Ref<GLTexture>& tex : textures)
		{
			if (m_textureNameCount.find(tex->Type) != m_textureNameCount.end())
			{
				m_textureNameCount[tex->Type]++;
			}
			else
			{
				m_textureNameCount[tex->Type] = 0;
				m_textures.push_back(tex);
			}
		}

		setupMesh();
	}

	Mesh(Vector<Vertex> vertices, Vector<uint> indices) : 
		m_vertices(vertices)
		, m_indices(indices)
	{
		setupMesh();
	}

	Mesh(Vector<Vertex> vertices, Vector<uint> indices, Vector<Ref<GLTexParams>> textures) :
		m_vertices(vertices)
		, m_indices(indices)
		, m_texturesToInit(textures)
	{

		for (auto& tex : m_texturesToInit)
		{
			if (m_textureNameCount.find(tex->Type) != m_textureNameCount.end())
			{
				m_textureNameCount[tex->Type]++;
			}
			else
			{
				m_textureNameCount[tex->Type] = 0;
			}
		}
	}

	Mesh(const Mesh& copy)
	{
		m_vertices = copy.m_vertices;
		m_indices = copy.m_indices;
		m_textures = copy.m_textures;
		m_vao = copy.m_vao;
		m_vbo = copy.m_vbo;
		m_ibo = copy.m_ibo;
		//m_texturesToInit = copy.m_texturesToInit;

		m_indexedDraw = copy.m_indexedDraw;
		m_instancedDraw = copy.m_instancedDraw;
		m_isInstanced = copy.m_isInstanced;
		//m_layout = copy.m_layout;
	}

	void setTextures(Vector<Ref<GLTexture>>& textures)
	{
		m_textures.insert(m_textures.end(), std::make_move_iterator(textures.begin()), std::make_move_iterator(textures.end()));
	}

	void setTexture(Ref<GLTexParams> texture)
	{
		/*m_textures.emplace_back(texture);*/
		m_textures.emplace_back(std::make_shared<GLTexture>(texture));
	}

	void setTexture(Ref<GLTexture> texture)
	{
		m_textures.push_back(texture);
	}

	void Draw(Shader& shader)
	{
		PROFILE_SCOPE("MeshDraw");

		//DYNAMIC_ASSERT(m_textures.size() > 0, "No textures attached to mesh");

		//needs to be an int because it is used to set the shader param and textures are ints
		int totalTexCount = 0;
		for (auto& texture : m_textures)
		{
			for (uint i = 0; i <= m_textureNameCount[texture->Type]; ++i)
			{
				shader.setUniform(texture->Type + std::to_string(i), totalTexCount);
				texture->Bind(totalTexCount);
				++totalTexCount;
			}
		}

		if (m_isInstanced)
		{
			g_renderer.DrawInstanced(m_instancedDraw);
		}
		else
		{
			g_renderer.DrawIndexed(m_indexedDraw);
		}

		//for (Ref<GLTexture>& tex : m_textures)
		//	tex->Unbind();
	}

	void setVertices(Vector<Vertex>& vertices)
	{
		m_vertices = vertices;
	}

	void setIndices(Vector<uint>& indices)
	{
		m_indices = indices;
	}

	void initTextures()
	{
		for (Ref<GLTexParams>& params : m_texturesToInit)
		{
			m_textures.push_back(std::make_shared<GLTexture>(params));
		}

		m_texturesToInit.clear();
	}

	void clearInitTextures()
	{
		m_texturesToInit.clear();
	}

	const Vector<Ref<GLTexParams>>& getTextureRequests() { return m_texturesToInit; }
	Vector<uint> getIndices() const { return m_indices; }
	Vector<Vertex> getVertices() const { return m_vertices; }

	void setInstanced(bool instanced, uint numInstances = 0) 
	{ 
		m_isInstanced = instanced; 
		if(instanced)
		{ 
			m_instancedDraw.instanceCount = numInstances;
			m_instancedDraw.indexBuffer = m_ibo;
			m_instancedDraw.vertexArrayBuffer = m_vao;
			m_instancedDraw.indexCount = m_indices.size();
		}
	}
	bool getIsInstanced() { return m_isInstanced; }

	void setupMesh()
	{
		PROFILE_FUNCTION();

		FATAL_ASSERT(m_indices.size() > 0 && m_vertices.size() > 0, "Attempting to create a mesh with no indices and vertices not set!");
		m_vao = std::make_shared<GLVertexArray>();
		m_vao->Bind();
		m_vbo = std::make_shared<GLVertexBuffer>(m_vertices.data(), m_vertices.size() * sizeof(Vertex));
		m_ibo = std::make_shared<GLIndexBuffer>(m_indices.data(), m_indices.size());

		GLVertexBufferLayout layout;
		layout.Push<Vertex>(1);

		m_vao->AddBuffer(*m_vbo, layout);

		//setup Draws. Kinda wasteful but whatever

		m_instancedDraw.indexBuffer = m_ibo;
		m_instancedDraw.vertexArrayBuffer = m_vao;
		m_instancedDraw.indexCount = m_indices.size();

		m_indexedDraw.primitiveCount = m_indices.size();
		m_indexedDraw.vertexArrayBuffer = m_vao;
		m_indexedDraw.indexBuffer = m_ibo;

		
	}

	const GLIndexedDrawCall& getIndexedDrawCall() const { return m_indexedDraw; }

	Vector<Ref<GLTexture>> getTextures()
	{
		return m_textures;
	}

private:
	//cpu copies
	Vector<Vertex> m_vertices;
	Vector<uint> m_indices;
	Vector<Ref<GLTexture>> m_textures;
	Vector<Ref<GLTexParams>> m_texturesToInit;
	bool m_isInstanced = false;
	//incase of multiple textures
	HashMap<string, uint> m_textureNameCount;
	
	GLIndexedDrawCall m_indexedDraw;
	GLInstancedDrawCall m_instancedDraw;

	Ref<GLVertexArray> m_vao;
	Ref<GLVertexBuffer> m_vbo;
	Ref<GLIndexBuffer> m_ibo;

};

