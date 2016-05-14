#include "Model.h"
#include <iostream>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#define SLOW_LAUNCH 1

void Model::updateAnimation(float speedFactor, int take, float currTime, glm::mat4x4 worldMat)
{

	/**
	More efficient algorithm:
	Save the frame i was just at. Save the take along wiht the frame.

	**/

	worldMat = glm::mat4();
	//Keep this on watch...
	float targetTime = speedFactor * currTime;
	std::vector<sKeyFrame> tempFrames;
	mpJointList;
	for (int i = 0; i < mpJointList.size(); i++)
	{
		float jointMaxTime = mpJointList[i].keyFramesByTake[take].back().keyTime;
		targetTime = std::fmod(targetTime, jointMaxTime);
		//Find the right keyframe based on time
		int currKeyIndex;
		
		const int frameCount = mpJointList[i].keyFramesByTake[take].size();
		
		//will pick the key with the time value that 
		//represents the "currTime" the best.
		int closestKey;
		float prevDiff = -1337;
		for (int j = 0; j < frameCount; j++)
		{
			if (prevDiff == -1337)
			{	 
				closestKey = j;
				prevDiff = abs(targetTime - mpJointList[i].keyFramesByTake[take][j].keyTime);
			}
			else {
				float currDiff = abs(targetTime - mpJointList[i].keyFramesByTake[take][j].keyTime);
				if (currDiff < prevDiff)
				{
					closestKey = j;
					prevDiff = currDiff;
				}
			}
			if (mpJointList[i].keyFramesByTake[take][j].keyTime == targetTime)
			{
				closestKey = j;
				break;
			}
			//mpJointList[i].keyFramesByTake[0][take][j];
		}
		/**
		Now save this value in a "temporary final list of frames"
		**/
		
		tempFrames.push_back(mpJointList[i].keyFramesByTake[take][closestKey]);
	}
	//Find the root joint.
	int rootKey = 0;
	for (int j = 0; j < mpJointList.size(); j++)
	{
		//If the joint has no parentjoint... That is: if it's the root.
		if (mpJointList[j].jointData.parentJointID < 0)
		{
			rootKey = j;
		}
	}


	recursiveUpdateJointMatrixList(worldMat, tempFrames, rootKey);
}

void shit() {
	/**
	Now multiply "The Root" with the current keyframe-values, and then multiply that matrix with the worldpos-values.
	The root now has a "final transform".
	Then for each root-child, multiply it's bind matrix with the keyframematrix, and then with it's parent matrix.

	But first: find the root-joint.

	bindpose * keyframe * inversebindpose

	GlobalInverseBindPose -> One for each joint.
	Multiply it with all of the matrix transform matrices
	You multiply it with all of the keyFrameMatrices.

	KEYFRAME * INVERSEBINDPOSE! THAS IT!
	**/
	/**
	Perform the requisite transforms on the root.
	We only care about the rotation.
	**/

	/**
	1. Get the key-transform for root.
	2. InverseBIndPose on it
	3. Multiply it with the input "transform-vector"
	**/
	//float tempInvBindPose[16];
	//for (int i = 0; i < 16; i++)
	//	tempInvBindPose[i] = mpJointList[rootKey].jointData->globalBindPoseInverse[i];
	//
	//glm::mat4 invBPose = convertToMat4(tempInvBindPose);

	////Now get the key-transform
	//glm::mat4 keySMat = convertToScaleMat(tempFrames[rootKey].keyScale);
	//glm::mat4 keyRMat = convertToRotMat(tempFrames[rootKey].keyRotate);
	//glm::mat4 keyTMat = convertToTransMat(tempFrames[rootKey].keyPos);
	//glm::mat4 keyTransform = keySMat * keyRMat * keyTMat;
	////Now "remove" the bindpose from the joint
	///**
	//Inversebindpose MIGHT mean that all of the joints enter their "origo", wherefrom they can get scaled, rotated, etc
	//without lokking funny.
	//**/
	//glm::mat4 pureKeyTransform = keyTransform * invBPose;
	////Now modify it with the worldMatrix
	//glm::mat4 finalRootMatrix = pureKeyTransform * worldMat;
	//
	////Saving the root matrix in the matrix array.
	//jointMatrixList[rootKey] = finalRootMatrix;

	//Now loop through the root's children, and apply the root-matrix to their
	//"final transform". Then loop through the children's children and apply 
	//that "final transform" to them. Etc...
}

bool Model::load( Assets* assets, std::string file )
{
	bool result = true;
	MoleReader assetData;
	
	assetData.readFromBinary( file.c_str() );

	/**
	Will now get all of the joints and their keyframes and their modelchildren into a single list, and order them based on jointID.
	I don't have to care TOO much about copying. I'll save memory-space anyway if I let go of the library-reader
	file. Don't have to keep hold of millions of vertices.
	**/

	const sMainHeader mainHeader = assetData.getMainHeader();
	for (int i = 0; i < mainHeader.meshCount; i++)
	{
		sModelMesh tempMesh;
		sMesh currMesh = assetData.getMesh(i);
		int jointCount = currMesh.jointCount;
		for (int j = 0; j < jointCount; j++)
		{
			sModelJoint tempJoint;
			const sJoint currJoint = assetData.getJoint(i, j);

			for (int k = 0; k < currJoint.animationStateCount; k++)
			{
				const std::vector<sKeyFrame> currFrameList = assetData.getKeyList(i, j, k);
				tempJoint.keyFramesByTake.push_back(currFrameList);
			}
			tempJoint.jointData = currJoint;
			if(currJoint.meshChildCount > 0)
				tempJoint.meshChildren = assetData.getJointMeshChildList(i, j);
			mpJointList.push_back(tempJoint);
		}
		std::vector<sMChildHolder> meshChildListHolder = assetData.getMeshChildList();

		tempMesh.meshChildren = meshChildListHolder[i].meshChildList;//meshChildListHolder[0][i].meshChildList;
		tempMesh.meshData = currMesh;
		mpMeshList.push_back(tempMesh);
	}

	sortJointsByID();
	mpJointList;
	if(mpJointList.size() > 0)
		makeJointHierarchy();
	jointMatrixList.resize(mpJointList.size());
	
	mMeshes = assetData.getMeshList().size();
	mMaps = assetData.getMaterialList().size();

	mpMeshes = new Mesh[mMeshes];
	mpDiffuseMaps = new Texture*[mMaps];
	mpSpecularMaps = new Texture*[mMaps];
	mpNormalMaps = new Texture*[mMaps];

	for( int i=0; i<mMeshes && result; i++ )
		result = result && mpMeshes[i].load( &assetData, i );

	for (int i = 0; i < mMaps && result; i++)
	{
		const sMaterial material = assetData.getMaterial(i);
		mpDiffuseMaps[i] = assets->load<Texture>(material.diffuseTexture);
		mpSpecularMaps[i] = assets->load<Texture>(material.specularTexture);
		mpNormalMaps[i] = assets->load<Texture>(material.normalTexture);
	
		if (mpDiffuseMaps[i] == nullptr)
		{
			mpDiffuseMaps[i] = assets->load<Texture>("Models/cube.png");
		}
	}

	return result;
}

void Model::unload()
{
	for (int i = 0; i < mMeshes; i++)
		mpMeshes[i].unload();

	delete[] mpMeshes;
	delete[] mpDiffuseMaps;
	delete[] mpSpecularMaps;
	delete[] mpNormalMaps;

	for (int i = 0; i < mpJointList.size(); i++) {
		mpJointList[i].jointChildren.clear();
		mpJointList[i].keyFramesByTake.clear();
		mpJointList[i].meshChildren.clear();
	}
	
	mMeshes = mMaps = 0;

	// No need to unload the textures, Assets will take care of that
}

void Model::drawAni()
{
	for (int i = 0; i < mMeshes; i++)
	{
		int material = mpMeshes[i].getMaterialIndex();

		mpDiffuseMaps[material]->bind(GL_TEXTURE0);

		Texture* spec = mpSpecularMaps[material];
		if (spec)
			spec->bind(GL_TEXTURE1);
		else
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		Texture* norm = mpNormalMaps[material];
		if (norm)
			norm->bind(GL_TEXTURE2);
		else
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		mpMeshes[i].draw();
	}
}

void Model::drawNonAni()
{
	for (int i = 0; i < mMeshes; i++)
	{
		int material = mpMeshes[i].getMaterialIndex();
		mpDiffuseMaps[material]->bind(GL_TEXTURE0);
		Texture* spec = mpSpecularMaps[material];
		if (spec)
			spec->bind(GL_TEXTURE1);
		else
		{
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		Texture* norm = mpNormalMaps[material];
		if (norm)
			norm->bind(GL_TEXTURE2);
		else
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		mpMeshes[i].drawNonAni();
	}
}

Texture* Model::getDiffuseMap(int index) const
{
	return mpDiffuseMaps[index];
}

Texture* Model::getSpecularMap(int index) const
{
	return mpSpecularMaps[index];
}

Texture* Model::getNormalMap(int index) const
{
	return mpNormalMaps[index];
}

void Model::sortJointsByID()
{
	std::vector<sModelJoint> newList;
	int searchID = 0;
	for (int i = 0; i < mpJointList.size(); i++)
	{
		if (mpJointList[i].jointData.jointID == searchID)
		{
			newList.push_back(mpJointList[i]);
			searchID++;
			//This little fella restarts the loop.
			//When the loop's finally done, we've 
			//got a new, beautiful list.
			i = -1;
		}
	}
	mpJointList = newList;
}

void Model::makeJointHierarchy()
{
	/**
	How will the joint hierarchy be saved? Well you tell me.
	**/
	int rootKey;
	for(int i = 0; i < mpJointList.size(); i++)
	{
		if (mpJointList[i].jointData.parentJointID < 0)
			rootKey = i;
	}
	recursiveMakeJointHierarchy(rootKey);
}

void Model::recursiveMakeJointHierarchy(int parentID)
{
	for (int j = 0; j < mpJointList.size(); j++)
	{
		if (mpJointList[j].jointData.parentJointID == parentID)
		{
			//Add it as child.
			mpJointList[parentID].jointChildren.push_back(mpJointList[j].jointData.jointID);
			recursiveMakeJointHierarchy(j);
		}
	}
}

void Model::recursiveUpdateJointMatrixList(glm::mat4 parentTransformMatrix, std::vector<sKeyFrame> tempFrames, int currJointID)
{
	//The transform has to apply even when there's no children.
	mpJointList[currJointID].jointData;

	/*float tempInvBindPose[16];
	for (int i = 0; i < 16; i++)
		tempInvBindPose[i] = mpJointList[currJointID].jointData->globalBindPoseInverse[i];
	glm::mat4 invBPose = convertToMat4(tempInvBindPose);*/

	glm::mat4 invBPose = glm::make_mat4( mpJointList[currJointID].jointData.globalBindPoseInverse );

	//Now get the key-
	glm::mat4 keyRMat = convertToRotMat(tempFrames[currJointID].keyRotate);
	glm::mat4 keySMat= convertToScaleMat(tempFrames[currJointID].keyScale);	
	glm::mat4 keyTMat = convertToTransMat(tempFrames[currJointID].keyPos);

	//glm::mat4 invinv = glm::inverse( invBPose );

	//glm::mat4 keyTransform = keySMat * keyRMat * keyTMat;
	//glm::mat4 keyTransform = keyTMat * keyRMat * keySMat;
	//VVVVVVVVVVVVVVV THIS WORKS WELL REMEMBER
	glm::mat4 keyTransform = parentTransformMatrix *  keyTMat * keySMat * keyRMat;
	//glm::mat4 keyTransform = parentTransformMatrix * keyTMat * keySMat * keyRMat;
	//Now "remove" the bindpose from the joint
	glm::mat4 pureKeyTransform = keyTransform * invBPose;
	//glm::mat4 pureKeyTransform = invBPose * keyTransform;
	glm::mat4 finalTransform = pureKeyTransform;
	//glm::mat4 finalTransform = keyTransform;// * parentTransformMatrix;

	

	jointMatrixList[currJointID] = finalTransform;

	for (int i = 0; i < mpJointList[currJointID].jointChildren.size(); i++)
	{
		//glm::mat4 junky;
		//recursiveUpdateJointMatrixList(junky, tempFrames, mpJointList[currJointID].jointChildren[i]);
		//VVVVVVVVVVVVVVVVVVVVVVVVV THIS ONE WORKS WELL REMEMBER
		recursiveUpdateJointMatrixList(keyTransform, tempFrames, mpJointList[currJointID].jointChildren[i]);
		
		//recursiveUpdateJointMatrixList(parentTransformMatrix, tempFrames, mpJointList[currJointID].jointChildren[i]);

		//recursiveUpdateJointMatrixList(parentTransformMatrix, tempFrames, mpJointList[currJointID].jointChildren[i]);
	}
	/**
	THink I've gotta transform the children so that they are in "spaces related to their parent's"
	**/
}

glm::mat4 Model::convertToTransMat(float inputArr[3])
{
	//glm::mat4 output =
	//{
	//	1.f, 0.f, 0.f, inputArr[0],
	//	0.f, 1.f, 0.f, inputArr[1],
	//	0.f, 0.f, 1.f, inputArr[2],
	//	0.f, 0.f, 0.f, 1.f
	//};
	glm::mat4 output =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		inputArr[0],  inputArr[1], inputArr[2], 1.f
	};

	return output;
}

glm::mat4 Model::convertToRotMat(float inputArr[3])
{
	using namespace glm;
	mat4 rotX = 
	{
		1.f,			0.f,					0.f,				0.f,
		0.f,		    cosf(inputArr[0]),	    sinf(inputArr[0]),	0.f,
		0.f,		    -sinf(inputArr[0]),     cosf(inputArr[0]),  0.f,
		0.f,			0.f,					0.f,				1.f
	};
	mat4 rotY =
	{
		cosf(inputArr[1]),	0.f,			-sinf(inputArr[1]),		0.f,
		0.f,				1.f,			0.f,					0.f,
		sinf(inputArr[1]),  0.f,			cosf(inputArr[1]),		0.f,
		0.f,				0.f,			0.f,					1.f
	};
	mat4 rotZ =
	{
		cosf(inputArr[2]),	sinf(inputArr[2]),		0.f,				0.f,
		-sinf(inputArr[2]), cosf(inputArr[2]),		0.f,				0.f,
		0.f,				0.f,					1.f,				0.f,
		0.f,				0.f,					0.f,				1.f
	};

	//mat4 rotMatrix = rotX * rotY * rotZ;
	
	//return rotMatrix;
	return rotX * rotY * rotZ;
}

glm::mat4 Model::convertToScaleMat(float inputArr[3])
{
	glm::mat4 output = 
	{
		inputArr[0],			0.f,			0.f,	 0.f,
				0.f,	inputArr[1],			0.f,	 0.f,
				0.f,			0.f,	inputArr[2],	 0.f,
				0.f,			0.f,	        0.f,	 1.f
	};
	return output;
}

glm::mat4 Model::convertToMat4(float inputArr[16])
{
	glm::mat4 output =
	{
		inputArr[0], inputArr[4], inputArr[8], inputArr[12],
		inputArr[1], inputArr[5], inputArr[9], inputArr[13],
		inputArr[2], inputArr[6], inputArr[10], inputArr[14],
		inputArr[3], inputArr[7], inputArr[11], inputArr[15]
	};
	/*glm::mat4 output =
	{
		inputArr[0], inputArr[1], inputArr[2], inputArr[3],
		inputArr[4], inputArr[5], inputArr[6], inputArr[7],
		inputArr[8], inputArr[9], inputArr[10], inputArr[11],
		inputArr[12], inputArr[13], inputArr[14], inputArr[15]
	};*/

	return output;
}

Model& Model::operator=( const Model& ref )
{
	return *this;
}

Model::Model( const Model& ref )
{
}

Model::Model()
{
}

Model::~Model()
{
	//delete this->mpMeshes;
}