#pragma once
#include <Common/CommonTypes.h>
#include <Mesh/Mesh.h>

class GeoGen
{
public:
	static Mesh CreateBox(float width, float height, float depth, uint numSubdivisions);
	static Mesh CreateSphere(float radius, uint sliceCount, uint stackCount);
	static Mesh CreateGeoSphere(float radius, uint numSubdivisions);
	static Mesh CreateCylinder(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount);
	static Mesh CreateGrid(float width, float depth, uint m, uint n);
	static Mesh CreateQuad(float x, float y, float w, float h, float depth);
private:
	static void Subdivide(Mesh& mesh);
	static Vertex MidPoint(const Vertex& v0, const Vertex& v1);
	static void BuildCylinderTop(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount, Mesh& mesh);
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint sliceCount, uint stackCount, Mesh& mesh);
};