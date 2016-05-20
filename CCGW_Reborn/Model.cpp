#include "Model.h"
#include <iostream>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#define SLOW_LAUNCH 1
/**
Seamless blending between animations:
Have a list of "take-weights".
The current animation's weight "increases quickly" to 100%,
while the other animations "decrease as quickly" to 0%.
TempFramesOver && TempFramesUnder will contain key-values that are "combinations" of the different "active" layers.

A consequence of this blending would be that the below code would run for all the animation layers all the time (only a "if weight == 0"
limiting it). The code "could" run the risk of making lag happen.
**/
void Model::updateAnimation( std::vector<glm::mat4>& jointMatrixList, float speedFactor, int take, float currTime)
{
	if (Model::mpJointList.size() > 0)
	{
		/**
		More efficient algorithm:
		Save the frame i was just at. Save the take along wiht the frame.

		**/

		glm::mat4 worldMat = glm::mat4();
		//Keep this on watch...
		float targetTime = speedFactor * currTime;
		tempFramesUnder.clear();
		tempFramesOver.clear();
		mpJointList;

		int njoints = mpJointList.size();
		for (int i = 0; i < njoints; i++)
		{
			Model::sModelJoint& joint = mpJointList[i];
			std::vector<sKeyFrame>& keyframes = joint.keyFramesByTake[take];
			const int frameCount = mpJointList[i].keyFramesByTake[take].size();

			//if( mpJointList[i].keyFramesByTake[take].size() <= 0 )
			if(frameCount <= 0 )
				continue;

			//float jointMaxTime = mpJointList[i].keyFramesByTake[take].back().keyTime;
			float jointMaxTime = keyframes.back().keyTime;

			targetTime = std::fmod(targetTime, jointMaxTime);
			//Find the right keyframe based on time
			int currKeyIndex;
			//const int frameCount = mpJointList[i].keyFramesByTake[take].size();

			int closestKeyPos;
			int closestKeyNeg;
			float prevDiffPos = -1337;
			float prevDiffNeg = -1337;

			for (int j = 0; j < frameCount; j++)
			{
				if (targetTime < 0.01f)
				{
					closestKeyPos = 0;
					closestKeyNeg = 0;
					break;
				}
				if ((jointMaxTime - targetTime) < 0.01f ) 
				{
					closestKeyPos = frameCount - 1;
					closestKeyNeg = frameCount - 1;
					break;
				}
				if ((prevDiffPos == -1337) && (prevDiffNeg == -1337))
				{
					closestKeyPos = j;
					closestKeyNeg = j;
					//float diff = abs(targetTime - mpJointList[i].keyFramesByTake[take][j].keyTime);
					float diff = abs(targetTime - keyframes[j].keyTime);
					prevDiffPos = diff;
					prevDiffNeg = -10000000000.f;
				}
				else {
					//float currDiff = targetTime - mpJointList[i].keyFramesByTake[take][j].keyTime;
					float currDiff = targetTime - keyframes[j].keyTime;

					//if keyTime was larger than targetTime
					if ((currDiff > prevDiffNeg) && (currDiff < 0.f))
					{
						closestKeyNeg = j;
						prevDiffNeg = currDiff;
					}
					//if keyTime was smaller than targetTime
					if ((currDiff < prevDiffPos) && (currDiff > 0.f))
					{
						closestKeyPos = j;
						prevDiffPos = currDiff;
					}
				}

				//if (mpJointList[i].keyFramesByTake[take][j].keyTime == targetTime)
				if( keyframes[j].keyTime == targetTime )
				{
					closestKeyNeg = j;
					closestKeyPos = j;
					break;
				}
				//mpJointList[i].keyFramesByTake[0][take][j];
			}

			/**
			Now save this value in a "temporary final list of frames"
			**/

	
			tempFramesUnder.push_back(mpJointList[i].keyFramesByTake[take][closestKeyPos]);
			tempFramesOver.push_back(mpJointList[i].keyFramesByTake[take][closestKeyNeg]);
		}
		//Find the root joint.
		int rootKey = 0;
		//for (int j = 0; j < mpJointList.size(); j++)
		for( int j=0; j<njoints; j++ )
		{
			//If the joint has no parentjoint... That is: if it's the root.
			if (mpJointList[j].jointData.parentJointID < 0)
			{
				rootKey = j;
			}
		}

		if( tempFramesOver.size() > 0 && tempFramesUnder.size() > 0 )
			recursiveUpdateJointMatrixList(jointMatrixList, worldMat, tempFramesUnder, tempFramesOver, targetTime, rootKey);
	}
}

bool Model::load(Assets* assets, std::string file)
{
	bool result = true;
	MoleReader assetData;

	assetData.readFromBinary(file.c_str());

	/**
	Will now get all of the joints and their keyframes and their modelchildren into a single list, and order them based on jointID.
	I don't have to care TOO much about copying. I'll save memory-space anyway if I let go of the library-reader
	file. Don't have to keep hold of millions of vertices.
	**/

	int maxAnimationStateCount = 0;

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

			maxAnimationStateCount = currJoint.animationStateCount;

			for (int k = 0; k < currJoint.animationStateCount; k++)
			{
				const std::vector<sKeyFrame> currFrameList = assetData.getKeyList(i, j, k);
				tempJoint.keyFramesByTake.push_back(currFrameList);
			}
			tempJoint.jointData = currJoint;
			if (currJoint.meshChildCount > 0)
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
	if (mpJointList.size() > 0)
		makeJointHierarchy();
	//jointMatrixList.resize(mpJointList.size());

	mMeshes = assetData.getMeshList().size();
	mMaps = assetData.getMaterialList().size();

	mpMeshes = new Mesh[mMeshes];
	mpDiffuseMaps = new Texture*[mMaps];
	mpSpecularMaps = new Texture*[mMaps];
	mpNormalMaps = new Texture*[mMaps];

	for (int i = 0; i<mMeshes && result; i++)
		result = result && mpMeshes[i].load(&assetData, i);

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

	for( int curAnimation=0; curAnimation<maxAnimationStateCount; curAnimation++ )
	{
		sAnimation animation = { curAnimation };

		for( int curMesh=0; curMesh < mainHeader.meshCount; curMesh++ )
		{
			const sMesh mesh = assetData.getMesh(curMesh);
			for( int curJoint=0; curJoint < mesh.jointCount; curJoint++ )
			{
				const sJoint joint = assetData.getJoint(curMesh,curJoint);
				if( joint.animationStateCount >= curAnimation )
				{
					const std::vector<sKeyFrame> keyframes = assetData.getKeyList(curMesh,curJoint,curAnimation);
					if( keyframes.size() > 0 )
						if( keyframes.back().keyTime > animation.mDuration )
							animation.mDuration = keyframes.rbegin()->keyTime;
				}
			}
		}

		mAnimations.push_back(animation);
	}

	int frameSize = 0;
	for (int i = 0; i < mpJointList.size(); i++)
		for (int j = 0; j < mpJointList[i].keyFramesByTake.size(); j++)
			frameSize += mpJointList[i].keyFramesByTake[j].size();

	tempFramesOver.reserve(frameSize);
	tempFramesUnder.reserve(frameSize);

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

sAnimation* Model::getAnimation( int index )
{
	if( index < mAnimations.size() )
		return ( mAnimations.data() + index );
	return nullptr;
}

int Model::getAnimationCount() const
{
	return mAnimations.size();
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
	for (int i = 0; i < mpJointList.size(); i++)
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

////The transform has to apply even when there's no children.
////mpJointList[currJointID].jointData;
//
///*float tempInvBindPose[16];
//for (int i = 0; i < 16; i++)
//tempInvBindPose[i] = mpJointList[currJointID].jointData->globalBindPoseInverse[i];
//glm::mat4 invBPose = convertToMat4(tempInvBindPose);*/
//
//glm::mat4 invBPose = glm::make_mat4(mpJointList[currJointID].jointData.globalBindPoseInverse);
//
////Now get the key-
//glm::mat4 keyRMat = convertToRotMat(tempFrames[currJointID].keyRotate);
//glm::mat4 keySMat = convertToScaleMat(tempFrames[currJointID].keyScale);
//glm::mat4 keyTMat = convertToTransMat(tempFrames[currJointID].keyPos);
//
////glm::mat4 invinv = glm::inverse( invBPose );
//
////glm::mat4 keyTransform = keySMat * keyRMat * keyTMat;
////glm::mat4 keyTransform = keyTMat * keyRMat * keySMat;
////VVVVVVVVVVVVVVV THIS WORKS WELL REMEMBER
//glm::mat4 keyTransform = parentTransformMatrix *  keyTMat * keySMat * keyRMat;
////glm::mat4 keyTransform = parentTransformMatrix * keyTMat * keySMat * keyRMat;
////Now "remove" the bindpose from the joint
//glm::mat4 pureKeyTransform = keyTransform * invBPose;
////glm::mat4 pureKeyTransform = invBPose * keyTransform;
//glm::mat4 finalTransform = pureKeyTransform;
////glm::mat4 finalTransform = keyTransform;// * parentTransformMatrix;
//
//
//
//jointMatrixList[currJointID] = finalTransform;
//
//for (int i = 0; i < mpJointList[currJointID].jointChildren.size(); i++)
//{
//	//glm::mat4 junky;
//	//recursiveUpdateJointMatrixList(junky, tempFrames, mpJointList[currJointID].jointChildren[i]);
//	//VVVVVVVVVVVVVVVVVVVVVVVVV THIS ONE WORKS WELL REMEMBER
//	recursiveUpdateJointMatrixList(keyTransform, tempFrames, mpJointList[currJointID].jointChildren[i]);
//
//	//recursiveUpdateJointMatrixList(parentTransformMatrix, tempFrames, mpJointList[currJointID].jointChildren[i]);
//
//	//recursiveUpdateJointMatrixList(parentTransformMatrix, tempFrames, mpJointList[currJointID].jointChildren[i]);
//}
///**
//THink I've gotta transform the children so that they are in "spaces related to their parent's"
//**/

/**
iVal will be multiplied with arr1. 1-iVal with arr2
**/
void Model::myLerp(float arr1[3], float arr2[3], float fillArr[3], float iVal)
{
	fillArr[0] = (arr1[0] * (1 - iVal)) + (arr2[0] * (iVal));
	fillArr[1] = (arr1[1] * (1 - iVal)) + (arr2[1] * (iVal));
	fillArr[2] = (arr1[2] * (1 - iVal)) + (arr2[2] * (iVal));
}


void Model::recursiveUpdateJointMatrixList( std::vector<glm::mat4>& jointMatrixList, const glm::mat4& parentTransformMatrix, std::vector<sKeyFrame>& tempFramesUnder, std::vector<sKeyFrame>& tempFramesOver, float currTime, int currJointID)
{
	if( currJointID >= tempFramesOver.size() )
	{
		jointMatrixList[currJointID] = parentTransformMatrix;
		return;
	}

	sKeyFrame& frameUnder = tempFramesUnder[currJointID];
	sKeyFrame& frameOver = tempFramesOver[currJointID];
	Model::sModelJoint& joint = mpJointList[currJointID];

	mpJointList;
	//glm::mat4 invBPose = glm::make_mat4(mpJointList[currJointID].jointData.globalBindPoseInverse);
	glm::mat4 invBPose = glm::make_mat4( joint.jointData.globalBindPoseInverse );

	//Now get the key-
	//glm::mat4 keyRMatUnder = convertToRotMat(tempFramesUnder[currJointID].keyRotate);
	//glm::mat4 keySMatUnder = convertToScaleMat(tempFramesUnder[currJointID].keyScale);
	//glm::mat4 keyTMatUnder = convertToTransMat(tempFramesUnder[currJointID].keyPos);

	//glm::mat4 keyRMatOver = convertToRotMat(tempFramesOver[currJointID].keyRotate);
	//glm::mat4 keySMatOver = convertToScaleMat(tempFramesOver[currJointID].keyScale);
	//glm::mat4 keyTMatOver = convertToTransMat(tempFramesOver[currJointID].keyPos);
	glm::mat4 interpolTrans;
	glm::mat4 interpolRot;
	glm::mat4 interpolScale;
	float newRotVals[3];
	float newTranVals[3];
	float newScaleVals[3];

	//float diffKeys = tempFramesOver[currJointID].keyTime - tempFramesUnder[currJointID].keyTime;
	float diffKeys = frameOver.keyTime - frameUnder.keyTime;
	if (diffKeys == 0)
	{
		//convertToTransMat(tempFramesUnder[currJointID].keyPos, &interpolTrans);
		//convertToRotMat(tempFramesUnder[currJointID].keyRotate, &interpolRot);
		//convertToScaleMat(tempFramesUnder[currJointID].keyScale, &interpolScale);

		convertToTransMat( frameUnder.keyPos, &interpolTrans );
		convertToRotMat( frameUnder.keyRotate, &interpolRot );
		convertToScaleMat( frameUnder.keyScale, &interpolScale );
	}
	else
	{
		//float diffUnderTime = abs(currTime - tempFramesUnder[currJointID].keyTime);
		float diffUnderTime = abs( currTime - frameUnder.keyTime );

		float underAffect = diffUnderTime / diffKeys;
		//float overAffect = 1.f - underAffect;
		//float overAffect = diffOverTime / diffKeys;

		//myLerp(tempFramesUnder[currJointID].keyRotate, tempFramesOver[currJointID].keyRotate, newRotVals, underAffect);
		myLerp( frameUnder.keyRotate, frameOver.keyRotate, newRotVals, underAffect );
		convertToRotMat(newRotVals, &interpolRot);

		//myLerp(tempFramesUnder[currJointID].keyPos, tempFramesOver[currJointID].keyPos, newTranVals, underAffect);
		myLerp( frameUnder.keyPos, frameOver.keyPos, newTranVals, underAffect );
		convertToTransMat(newTranVals, &interpolTrans);

		//myLerp(tempFramesUnder[currJointID].keyScale, tempFramesOver[currJointID].keyScale, newScaleVals, underAffect);
		myLerp( frameUnder.keyScale, frameOver.keyScale, newScaleVals, underAffect );
		convertToScaleMat(newScaleVals, &interpolScale);
	}

	/*interpolTrans = convertToTransMat(tempFramesUnder[currJointID].keyPos);
	interpolRot = convertToRotMat(tempFramesUnder[currJointID].keyRotate);
	interpolScale = convertToScaleMat(tempFramesUnder[currJointID].keyScale);*/

	//VVVVVVVVVVVVVVV THIS WORKS WELL REMEMBER
	//glm::mat4 keyTransform = parentTransformMatrix *  keyTMat * keySMat * keyRMat;
	//glm::mat4 TSRMat = convertToTSR(newTranVals, newScaleVals, newRotVals);
	//glm::mat4 keyTransform1 = parentTransformMatrix *  TSRMat;

	glm::mat4 keyTransform = parentTransformMatrix *interpolTrans * interpolScale * interpolRot;

	//glm::mat4 keyTransform = TSRMat * parentTransformMatrix;
	//glm::mat4 keyTransform = parentTransformMatrix * keyTMat * keySMat * keyRMat;
	//Now "remove" the bindpose from the joint

	glm::mat4 finalTransform = keyTransform * invBPose;

	//glm::mat4 pureKeyTransform = invBPose * keyTransform;

	//glm::mat4 finalTransform = keyTransform;// * parentTransformMatrix;

	jointMatrixList[currJointID] = finalTransform;

	int nchildren = joint.jointChildren.size();
	//for (int i = 0; i < mpJointList[currJointID].jointChildren.size(); i++)
	for( int i=0; i<nchildren; i++ )
	{
		//glm::mat4 junky;
		//recursiveUpdateJointMatrixList(junky, tempFrames, mpJointList[currJointID].jointChildren[i]);
		//VVVVVVVVVVVVVVVVVVVVVVVVV THIS ONE WORKS WELL REMEMBER
		//recursiveUpdateJointMatrixList(keyTransform, tempFrames, mpJointList[currJointID].jointChildren[i]);

		recursiveUpdateJointMatrixList( jointMatrixList, keyTransform, tempFramesUnder, tempFramesOver, currTime, mpJointList[currJointID].jointChildren[i]);

		//recursiveUpdateJointMatrixList(parentTransformMatrix, tempFrames, mpJointList[currJointID].jointChildren[i]);
		//recursiveUpdateJointMatrixList(parentTransformMatrix, tempFrames, mpJointList[currJointID].jointChildren[i]);
	}
	/**
	THink I've gotta transform the children so that they are in "spaces related to their parent's"
	**/
}

//glm::mat4 Model::convertToTransMat(float inputArr[3])
void Model::convertToTransMat(float inputArr[3], glm::mat4* result)
{
	//glm::mat4 output =
	//{
	//	1.f, 0.f, 0.f, inputArr[0],
	//	0.f, 1.f, 0.f, inputArr[1],
	//	0.f, 0.f, 1.f, inputArr[2],
	//	0.f, 0.f, 0.f, 1.f
	//};

	/*glm::mat4 output =
	{
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	inputArr[0],  inputArr[1], inputArr[2], 1.f
	};*/

	for (int i = 0; i < 3; i++)
		(*result)[3][i] = inputArr[i];
}

//glm::mat4 Model::convertToRotMat(float in[3])
void Model::convertToRotMat(float in[3], glm::mat4* result)
{
	using namespace glm;
	/*mat4 rotX =
	{
	1.f,			0.f,					0.f,				0.f,
	0.f,		    cosf(in[0]),	    sinf(in[0]),	0.f,
	0.f,		    -sinf(in[0]),     cosf(in[0]),  0.f,
	0.f,			0.f,					0.f,				1.f
	};
	mat4 rotY =
	{
	cosf(in[1]),	0.f,			-sinf(in[1]),		0.f,
	0.f,				1.f,			0.f,					0.f,
	sinf(in[1]),  0.f,			cosf(in[1]),		0.f,
	0.f,				0.f,			0.f,					1.f
	};
	mat4 rotZ =
	{
	cosf(in[2]),	sinf(in[2]),		0.f,				0.f,
	-sinf(in[2]), cosf(in[2]),		0.f,				0.f,
	0.f,				0.f,					1.f,				0.f,
	0.f,				0.f,					0.f,				1.f
	};

	mat4 rotMatrix = rotX * rotY * rotZ;*/

	//mat4 rotYZ = 
	//{
	//	cosf(in[1])*cosf(in[2]), cosf(in[1])*sinf(in[2]), -sinf(in[1]), 0.f,
	//	-sinf(in[2]), cosf(in[2]), 0.f, 0.f,
	//	sinf(in[1])*cosf(in[2]), sinf(in[1])*sinf(in[2]), cosf(in[1]), 0.f,
	//	0.f, 0.f, 0.f, 1.f
	//};
	//mat4 rotNewXYZ = rotX * rotYZ;

	*result =
	{
		cosf(in[1]) * cosf(in[2]), cosf(in[1]) * sinf(in[2]), -sinf(in[1]), 0.f,
		cosf(in[0]) * (-sinf(in[2])) + sinf(in[0]) * sinf(in[1]) * cosf(in[2]), cosf(in[0]) * cosf(in[2]) + sinf(in[0]) * sinf(in[1]) * sinf(in[2]), sinf(in[0]) * cosf(in[1]), 0.f,
		-sinf(in[0]) * (-sinf(in[2])) + cosf(in[0]) * sinf(in[1]) * cosf(in[2]), (-sinf(in[0])) * cosf(in[2]) + cosf(in[0]) * sinf(in[1]) * sinf(in[2]), cosf(in[0]) * cosf(in[1]), 0.f,
		0.f, 0.f, 0.f, 1.f
	};

	/*mat4 rotXYZ2 =
	{
	cosf(in[1])*cosf(in[2]), cosf(in[1]) * sinf(in[2]) * cos(in[0]) + -1.f * sinf(in[1]) * -1.f * sinf(in[0]), cosf(in[1]) * sinf(in[2]) * sinf(in[0]), 0.f,
	-1.f * sinf(in[2]), cosf(in[2]) * cosf(in[0]), cosf(in[2]) * sinf(in[0]), 0.f,
	sin(in[1])*cosf(in[2]), sinf(in[1])*sinf(in[2])*cosf(in[0]) + cosf(in[1]) * -1.f * sin(in[2]), sinf(in[1]) * sinf(in[2]) * sinf(in[0]) + cosf(in[1]) * cosf(in[0]), 0.f,
	0.f, 0.f, 0.f, 1.f
	};*/
	/*mat4 rotXYZ3 =
	{
	cosf(in[1])*cosf(in[2]), cosf(in[1]) * sinf(in[2]) * cos(in[0]) + sinf(in[1]) * sinf(in[0]), cosf(in[1]) * sinf(in[2]) * sinf(in[0]), 0.f,
	-sinf(in[2]), cosf(in[2]) * cosf(in[0]), cosf(in[2]) * sinf(in[0]), 0.f,
	sin(in[1])*cosf(in[2]), sinf(in[1])*sinf(in[2])*cosf(in[0]) + cosf(in[1]) * -sin(in[2]), sinf(in[1]) * sinf(in[2]) * sinf(in[0]) + cosf(in[1]) * cosf(in[0]), 0.f,
	0.f, 0.f, 0.f, 1.f
	};*/
	//return rotXYZ;
	//return rotXYZ3;
	//return rotMatrix;
	//return rotX * rotY * rotZ;

}

//glm::mat4 Model::convertToScaleMat(float inputArr[3])
void Model::convertToScaleMat(float inputArr[3], glm::mat4* result)
{
	/*glm::mat4 output =
	{
	inputArr[0],			0.f,			0.f,	 0.f,
	0.f,	inputArr[1],			0.f,	 0.f,
	0.f,			0.f,	inputArr[2],	 0.f,
	0.f,			0.f,	        0.f,	 1.f
	};
	return output;*/

	for (int i = 0; i < 3; i++)
		(*result)[i][i] = inputArr[i];
}

glm::mat4 Model::convertToTSR(float iT[3], float iS[3], float iR[3])
{
	//Translate
	//	1.f, 0.f, 0.f, 0.f,
	//	0.f, 1.f, 0.f, 0.f,
	//	0.f, 0.f, 1.f, 0.f,
	//	iT[0], iT[1], iT[2], 1.f
	//SCALE
	//		    iS[0],			0.f,			0.f,	 0.f,
	//			0.f,	        iS[1],			0.f,	 0.f,
	//			0.f,			0.f,	        iS[2],	 0.f,
	//			0.f,			0.f,	        0.f,	 1.f
	//ROTATE
	//cosf(in[1]) * cosf(in[2]), cosf(in[1]) * sinf(in[2]), -sinf(in[1]), 0.f,
	//cosf(in[0]) * (-sinf(in[2])) + sinf(in[0]) * sinf(in[1]) * cosf(in[2]), cosf(in[0]) * cosf(in[2]) + sinf(in[0]) * sinf(in[1]) * sinf(in[2]), sinf(in[0]) * cosf(in[1]), 0.f,
	//-sinf(in[0]) * (-sinf(in[2])) + cosf(in[0]) * sinf(in[1]) * cosf(in[2]), (-sinf(in[0])) * cosf(in[2]) + cosf(in[0]) * sinf(in[1]) * sinf(in[2]), cosf(in[0]) * cosf(in[1]), 0.f,
	//0.f, 0.f, 0.f, 1.f

	//SR
	/**
	iS[0] * (cosf(iR[1]) * cosf(iR[2])),                                                            iS[0] * (cosf(iR[1]) * sinf(iR[2])),                                                           iS[0] * (-sinf(iR[1])),              0.f,
	iS[1] * (cosf(iR[0]) * iS[1] * (-sinf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))),     iS[1] * ((cosf(iR[0]) * cosf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))),             iS[1] * sinf(iR[0]) * cosf(iR[1]),   0.f,
	iS[2] * ((-sinf(iR[0]) * (-sinf(iR[2]))) + (cosf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))),          iS[2] * (((-sinf(iR[0])) * cosf(iR[2])) + (cosf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))),          iS[2] * cosf(iR[0]) * cosf(iR[1]),   0.f,
	0.f,                                                                                  0.f,                                                                        0.f,                         1.f
	**/
	//TSR
	/**
	iT[0] * iS[0] * (cosf(iR[1]) * cosf(iR[2])),                                                            iT[0] * iS[0] * (cosf(iR[1]) * sinf(iR[2])),                                                           iT[0] * iS[0] * (-sinf(iR[1])),              0.f,
	iT[1] * iS[1] * (cosf(iR[0]) * iS[1] * (-sinf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))),     iT[1] * iS[1] * ((cosf(iR[0]) * cosf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))),             iT[1] * iS[1] * sinf(iR[0]) * cosf(iR[1]),   0.f,
	iT[2] * iS[2] * ((-sinf(iR[0]) * (-sinf(iR[2]))) + (cosf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))),          iT[2] * iS[2] * (((-sinf(iR[0])) * cosf(iR[2])) + (cosf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))),          iT[2] * iS[2] * cosf(iR[0]) * cosf(iR[1]),   0.f,
	0.f,                                                                                                   0.f,                                                                                                   0.f,                                         1.f

	**/
	/*glm::mat4 TSR =
	{
	iT[0] * iS[0] * (cosf(iR[1]) * cosf(iR[2])), iT[0] * iS[0] * (cosf(iR[1]) * sinf(iR[2])), iT[0] * iS[0] * (-sinf(iR[1])), 0.f,
	iT[1] * iS[1] * (cosf(iR[0]) * iS[1] * (-sinf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))), iT[1] * iS[1] * ((cosf(iR[0]) * cosf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))), iT[1] * iS[1] * sinf(iR[0]) * cosf(iR[1]), 0.f,
	iT[2] * iS[2] * ((-sinf(iR[0]) * (-sinf(iR[2]))) + (cosf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))), iT[2] * iS[2] * (((-sinf(iR[0])) * cosf(iR[2])) + (cosf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))), iT[2] * iS[2] * cosf(iR[0]) * cosf(iR[1]), 0.f,
	0.f, 0.f, 0.f, 1.f
	};*/

	glm::mat4 trans =
	{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		iT[0], iT[1], iT[2], 1.f
	};
	glm::mat4 rot =
	{
		cosf(iR[1]) * cosf(iR[2]), cosf(iR[1]) * sinf(iR[2]), -sinf(iR[1]), 0.f,
		cosf(iR[0]) * (-sinf(iR[2])) + sinf(iR[0]) * sinf(iR[1]) * cosf(iR[2]), cosf(iR[0]) * cosf(iR[2]) + sinf(iR[0]) * sinf(iR[1]) * sinf(iR[2]), sinf(iR[0]) * cosf(iR[1]), 0.f,
		-sinf(iR[0]) * (-sinf(iR[2])) + cosf(iR[0]) * sinf(iR[1]) * cosf(iR[2]), (-sinf(iR[0])) * cosf(iR[2]) + cosf(iR[0]) * sinf(iR[1]) * sinf(iR[2]), cosf(iR[0]) * cosf(iR[1]), 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	glm::mat4 scal =
	{
		iS[0],			0.f,			0.f,	 0.f,
		0.f,	        iS[1],			0.f,	 0.f,
		0.f,			0.f,	        iS[2],	 0.f,
		0.f,			0.f,	        0.f,	 1.f
	};

	glm::mat4 srlol = trans * scal * rot;

	glm::mat4 tsr =
	{
		iS[0] * (cosf(iR[1]) * cosf(iR[2])),                                                            iS[0] * (cosf(iR[1]) * sinf(iR[2])),                                                           iS[0] * (-sinf(iR[1])),              0.f,
		iS[1] * (cosf(iR[0]) * iS[1] * (-sinf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))),     iS[1] * ((cosf(iR[0]) * cosf(iR[2])) + (sinf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))),             iS[1] * sinf(iR[0]) * cosf(iR[1]),   0.f,
		iS[2] * ((-sinf(iR[0]) * (-sinf(iR[2]))) + (cosf(iR[0]) * sinf(iR[1]) * cosf(iR[2]))),          iS[2] * (((-sinf(iR[0])) * cosf(iR[2])) + (cosf(iR[0]) * sinf(iR[1]) * sinf(iR[2]))),          iS[2] * cosf(iR[0]) * cosf(iR[1]),   0.f,
		iT[0],                                                                                  iT[1],                                                                        iT[2],                         1.f
	};

	return tsr;
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

Model& Model::operator=(const Model& ref)
{
	return *this;
}

Model::Model(const Model& ref)
{
}

Model::Model()
{
}

Model::~Model()
{
	//delete this->mpMeshes;
}