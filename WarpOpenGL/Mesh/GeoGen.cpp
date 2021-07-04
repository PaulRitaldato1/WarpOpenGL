#include <Mesh/GeoGen.h>
#include <algorithm>
#include <Common/CommonTypes.h>

#define PI 3.141592654

Mesh GeoGen::CreateBox(float width, float height, float depth, uint numSubdivisions)
{
	PROFILE_FUNCTION();

	Vector<Vertex> v;
	v.resize(24);

	float w2 = 0.5 * width;
	float h2 = 0.5 * height;
	float d2 = 0.5 * depth;

	//front face
	v[0] = Vertex(glm::vec3(-w2, -h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[1] = Vertex(glm::vec3(-w2, +h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[2] = Vertex(glm::vec3(+w2, +h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[3] = Vertex(glm::vec3(+w2, -h2, -d2), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//back face
	v[4] = Vertex(glm::vec3(-w2, -h2, +d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[5] = Vertex(glm::vec3(+w2, -h2, +d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[6] = Vertex(glm::vec3(+w2, +h2, +d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[7] = Vertex(glm::vec3(-w2, +h2, +d2), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//top face
	v[8] = Vertex(glm::vec3(-w2, +h2, -d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[9] = Vertex(glm::vec3(-w2, +h2, +d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[10] = Vertex(glm::vec3(+w2, +h2, +d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[11] = Vertex(glm::vec3(+w2, +h2, -d2), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//bottom face
	v[12] = Vertex(glm::vec3(-w2, -h2, -d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[13] = Vertex(glm::vec3(+w2, -h2, -d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[14] = Vertex(glm::vec3(+w2, -h2, +d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[15] = Vertex(glm::vec3(-w2, -h2, +d2), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	//left face
	v[16] = Vertex(glm::vec3(-w2, -h2, +d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[17] = Vertex(glm::vec3(-w2, +h2, +d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[18] = Vertex(glm::vec3(-w2, +h2, -d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[19] = Vertex(glm::vec3(-w2, -h2, -d2), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
													   
	//right face									   
	v[20] = Vertex(glm::vec3(+w2, -h2, -d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[21] = Vertex(glm::vec3(+w2, +h2, -d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[22] = Vertex(glm::vec3(+w2, +h2, +d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	v[23] = Vertex(glm::vec3(+w2, -h2, +d2), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	
	Vector<uint> i;
	i.resize(36);

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	numSubdivisions = std::min<uint>(numSubdivisions, 6u);

	//Mesh mesh(v, i);
	//for (uint i = 0; i < numSubdivisions; ++i)
	//	Subdivide(mesh);

	return Mesh(v,i);
}

Mesh GeoGen::CreateSphere(float radius, uint sliceCount, uint stackCount)
{
	PROFILE_FUNCTION();

	Vector<Vertex> verts;
	Vertex topVertex(glm::vec3(0.0f, +radius, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Vertex botVertex(glm::vec3(0.0f, -radius, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));

	verts.push_back(topVertex);

	float phiStep = PI / stackCount;
	float thetaStep = 2.0f * PI / sliceCount;

	for (uint i = 1; i <= stackCount - 1; ++i)
	{
		float phi = i * phiStep;
		for (uint j = 0; j <= sliceCount; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;
			v.Position.x = radius * sin(phi) * cos(theta);
			v.Position.y = radius * cos(phi);
			v.Position.z = radius * sin(phi) * sin(theta);

			v.Tangent.x = -radius * sin(phi) * sin(theta);
			v.Tangent.y = 0.0f;
			v.Tangent.z = radius * sin(phi) * cos(theta);

			v.Tangent = glm::normalize(v.Tangent);
			v.Normal = glm::normalize(v.Position);
			
			v.TexCoords.x = theta / (2*PI);
			v.TexCoords.y = phi / PI;
			verts.push_back(v);
		}
	}

	verts.push_back(botVertex);

	Vector<uint> indices;
	for (uint i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint baseIndex = 1;
	uint ringVertexCount = sliceCount + 1;
	for (uint i = 0; i < stackCount - 2; ++i)
	{
		for (uint j = 0; j < sliceCount; ++j)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);

		}
	}

	uint southPoleIndex = (uint)verts.size() - 1;
	baseIndex = southPoleIndex - ringVertexCount;

	for (uint i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	return Mesh(verts, indices);
}

Mesh GeoGen::CreateGeoSphere(float radius, uint numSubdivisions)
{
	return Mesh();
}

Mesh GeoGen::CreateCylinder(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount)
{
	return Mesh();
}

Mesh GeoGen::CreateGrid(float width, float depth, uint m, uint n)
{
	PROFILE_FUNCTION();

	uint vertCount = m * n;
	uint faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5 * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	Vector<Vertex> verts;
	verts.resize(vertCount);

	for (uint i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (uint j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			verts[i * n + j].Position = glm::vec3(x, 0.0f, z);
			verts[i * n + j].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			verts[i * n + j].Tangent = glm::vec3(1.0f, 0.0f, 0.0f);

			verts[i * n + j].TexCoords.x = j * du;
			verts[i * n + j].TexCoords.y = j * dv;
		}
	}

	Vector<uint> indices;
	indices.resize(faceCount * 3);
	uint k = 0;
	for (uint i = 0; i < m - 1; ++i)
	{
		for (uint j = 0; j < n - 1; j++)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	return Mesh(verts, indices);
}

Mesh GeoGen::CreateQuad(float x, float y, float w, float h, float depth)
{
	PROFILE_FUNCTION();

	Vector<Vertex> verts;
	Vector<uint> indices;

	verts.resize(4);
	indices.resize(6);

	verts[0] = Vertex(
	glm::vec3(x, y - h, depth)
	, glm::vec3(0.0f, 0.0f, -1.0f)
	, glm::vec2(0.0f, 1.0f)
	, glm::vec3(1.0f, 0.0f, 0.0f)
	, glm::vec3(0.0f, 0.0f, 0.0f));

	verts[1] = Vertex(
		glm::vec3(x, y, depth)
		, glm::vec3(0.0f, 0.0f, -1.0f)
		, glm::vec2(0.0f, 0.0f)
		, glm::vec3(1.0f, 0.0f, 0.0f)
		, glm::vec3(0.0f, 0.0f, 0.0f));

	verts[2] = Vertex(
		glm::vec3(x+w, y, depth)
		, glm::vec3(0.0f, 0.0f, -1.0f)
		, glm::vec2(1.0f, 0.0f)
		, glm::vec3(1.0f, 0.0f, 0.0f)
		, glm::vec3(0.0f, 0.0f, 0.0f));

	verts[3] = Vertex(
		glm::vec3(x, y - h, depth)
		, glm::vec3(0.0f, 0.0f, -1.0f)
		, glm::vec2(1.0f, 1.0f)
		, glm::vec3(1.0f, 0.0f, 0.0f)
		, glm::vec3(0.0f, 0.0f, 0.0f));

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;

	indices[3] = 0;
	indices[4] = 2;
	indices[5] = 3;

	return Mesh(verts, indices);
}

void GeoGen::Subdivide(Mesh& mesh)
{
	PROFILE_FUNCTION();

	Vector<Vertex> vertCopy = mesh.getVertices();
	Vector<uint> indCopy = mesh.getIndices();

	Vector<Vertex> newVert;
	Vector<uint> newInd;

	uint numTris = (uint)indCopy.size()/3;
	for (uint i = 0; i < numTris; ++i)
	{
		Vertex v0 = vertCopy[indCopy[i * 3 + 0]];
		Vertex v1 = vertCopy[indCopy[i * 3 + 1]];
		Vertex v2 = vertCopy[indCopy[i * 3 + 2]];

		//get midpoints
		Vertex m0 = MidPoint(v0, v1);
		Vertex m1 = MidPoint(v1, v2);
		Vertex m2 = MidPoint(v0, v2);

		newVert.push_back(v0);
		newVert.push_back(v1);
		newVert.push_back(v2);
		newVert.push_back(m0);
		newVert.push_back(m1);
		newVert.push_back(m2);

		newInd.push_back(i * 6 + 0);
		newInd.push_back(i * 6 + 3);
		newInd.push_back(i * 6 + 5);

		newInd.push_back(i * 6 + 3);
		newInd.push_back(i * 6 + 4);
		newInd.push_back(i * 6 + 5);

		newInd.push_back(i * 6 + 5);
		newInd.push_back(i * 6 + 4);
		newInd.push_back(i * 6 + 2);

		newInd.push_back(i * 6 + 3);
		newInd.push_back(i * 6 + 1);
		newInd.push_back(i * 6 + 4);
	}

	mesh.setVertices(newVert);
	mesh.setIndices(newInd);
}

Vertex GeoGen::MidPoint(const Vertex& v0, const Vertex& v1)
{
	PROFILE_FUNCTION();

	glm::vec3 pos = 0.5f * (v0.Position + v1.Position);
	glm::vec3 normal = glm::normalize(0.5f * (v0.Normal + v1.Normal));
	glm::vec3 tan = glm::normalize(0.5f * (v0.Tangent + v1.Tangent));
	glm::vec2 tex = 0.5f * (v0.TexCoords + v1.TexCoords);

	return Vertex(pos, normal, tex, tan, glm::vec3(0.0f, 0.0f, 0.0f));
}

void GeoGen::BuildCylinderTop(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount, Mesh& mesh)
{

}

void GeoGen::BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount, Mesh& mesh)
{

}
