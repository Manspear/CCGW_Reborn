#pragma once

#include <vector>
#include <fbxsdk.h>

class Mesh
{
public:

	struct sVertexData
	{
		float vertexPos[3];
		float vertexNormal[3];
		float tangentNormal[3];
		float biTangentNormal[3];
		float vertexUV[2];
	};

	struct sTransformData
	{
		FbxAMatrix worldPos;
		FbxAMatrix worldRot;
		FbxAMatrix worldScale; 
	};

	struct sMaterialData
	{
		char* diffuseTexture;
		char* specularTexture;
		char* normalTexture;
	};

	sVertexData vertexData;

	std::vector<sVertexData> mpVertexList;
	std::vector<sMaterialData> mpMaterialList;

	Mesh();
	~Mesh();

private:
};

