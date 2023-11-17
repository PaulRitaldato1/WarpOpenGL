#pragma once
#include <Common/CommonTypes.h>
#include <Mesh/Mesh.h>

class GeoGen
{
public:
	static WarpMesh CreateBox(float Width, float Height, float Depth, uint NumSubdivisions);
	static Ref<WarpMesh> CreateSphere(float Radius, uint SliceCount, uint StackCount);
	static WarpMesh CreateGeoSphere(float Radius, uint NumSubdivisions);
	static WarpMesh CreateCylinder(float BottomRadius, float TopRadius, float Height, uint SliceCount, uint StackCount);
	static WarpMesh CreateGrid(float Width, float Depth, uint M, uint N);
	static Ref<WarpMesh> CreateQuad(float X, float Y, float W, float H, float Depth);
	static Ref<WarpMesh> CreateDefaultQuad();
private:
	static void Subdivide(WarpMesh& Mesh);
	static Vertex MidPoint(const Vertex& V0, const Vertex& V1);
	static void BuildCylinderTop(float BottomRadius, float TopRadius, float Height, uint SliceCount, uint StackCount, WarpMesh& Mesh);
	static void BuildCylinderBottomCap(float BottomRadius, float TopRadius, float Height, uint SliceCount, uint StackCount, WarpMesh& Mesh);
};