#pragma once

#include "Mesh.h"
#include "Texture.h"
#include <glm\gtx\matrix_operation.hpp>
#include <Mole\MoleReader.h>

class Model : public Asset
{
public:
	struct sModelJoint
	{
		sJoint jointData;
		std::vector<std::vector<sKeyFrame>> keyFramesByTake;
		std::vector<sMeshChild> meshChildren;
		std::vector<int> jointChildren;
	};

	struct sModelMesh
	{
		sMesh meshData;
		std::vector<sMeshChild> meshChildren;
	};

	std::vector<glm::mat4> jointMatrixList;

	void updateAnimation(float speedFactor, int take, float currTime, glm::mat4x4 worldMat);
	bool load(Assets* assets, std::string file);
	void unload();
	void drawAni();
	void drawNonAni();

	Texture* getDiffuseMap(int index) const;
	Texture* getSpecularMap(int index) const;
	Texture* getNormalMap(int index) const;

	Model& operator=(const Model& ref);
	Model(const Model& ref);
	Model();
	~Model();

private:
	Mesh* mpMeshes;
	int mMeshes;

	Texture **mpDiffuseMaps, **mpSpecularMaps, **mpNormalMaps;
	int mMaps;
	std::vector<sModelJoint> mpJointList;
	std::vector<sModelMesh> mpMeshList;
	std::vector<sKeyFrame> tempFramesOver;
	std::vector<sKeyFrame> tempFramesUnder;

	void sortJointsByID();
	void makeJointHierarchy();
	void recursiveMakeJointHierarchy(int parentID);
	void recursiveUpdateJointMatrixList(glm::mat4 parentTransformMatrix, std::vector<sKeyFrame>& tempFramesUnder, std::vector<sKeyFrame>& tempFramesOver, float currTime, int currJointID);
	//glm::mat4 convertToTransMat(float inputArr[3]);
	void convertToTransMat(float inputArr[3], glm::mat4* result);
	//glm::mat4 convertToRotMat(float inputArr[3]);
	void convertToRotMat(float inputArr[3], glm::mat4* result);
	//glm::mat4 convertToScaleMat(float inputArr[3]);
	void convertToScaleMat(float inputArr[3], glm::mat4* result);
	glm::mat4 convertToTSR(float iT[3], float iS[3], float iR[3]);
	void myLerp(float arr1[3], float arr2[3], float fillArr[3], float iVal);
	glm::mat4 convertToMat4(float inputArr[16]);
};