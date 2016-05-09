#include "Model.h"

#define SLOW_LAUNCH 1

void Model::updateAnimation(float speedFactor, int take, float & currTime, glm::mat4x4 worldMat)
{
	/**
	If we're gonna do blending we've gotta find the two keys that are closest
	to the currTime.
	One of those two keytimes must be "higher" than currTime, and the other "lower"
	than keyTime.
	But before blending, let's just take the key that's closest to the currTime.
	That way we'll see if the matrices work.

	How will we determine "where" on the animation we're currently?
	As soon as you press the "w"-button you start a "holddowntimer".
	That holddowntimer you send into this function.
	When the holddowntimer time exceeds this animation's "max time"
	the holddowntimer must reset so that the animationloop may reset.

	... How do you really make animations consistently run at 60 fps?
	Is it all time?
	Hmm... 60 frames per second. 60 frames. It says how many samples
	of the animation is needed...
	Animated at 24 fps, but game runnign at 60 fps...
	The framerate of the engine has nothing to do with the animation
	play speed.
	The animation plays separately. So how do we make animations run faster
	"dynamically" ? What does it mean for an animation to run faster?
	The animation WILL get sampled at the framerate that the engine's running at.
	What happens if "speedfactor" is multiplied with currTIme? You sample from a later stage in the animation.
	If you've held key for
	if speedfactor = 2
	0 1 2 3 4 5
	0 2 4 6 8 10
	Well look at that. It IS running consistently faster! Glorious.

	Hmm... currTime can "run out of scope" quite easily.
	Joints can and will have differing animationendtimes.
	The idéal would be to NEVER modify currTime directly from in here,
	but instead have one separate currTime for each joint.
	each Joint having a "reset currTime".
	But how to make currTime to NEVER run out of scope? Hmm... Math?
	yes math... But what math?

	50
	5
	"How many fives fit in there? 10! Easily dividable, no math rests.
	6
	5.5
	6 - 5.5 = 0.5

	40
	3.2
	40 - 3.2 = 36.8 <-- Not usable.
	40 / 3.2 = 12.5
	12.5 3.2's fit inside of 40.
	SO if we multiply 3.2 * 0.5 = 1.6
	It is... Maybe correct.
	Algorithm:
	the rest of currTime % maxKeyTime
	is "the joint time".
	fmod seems to do the trick.
	**/

	/**
	More efficient algorithm:
	Save the frame i was just at. Save the take along wiht the frame.

	**/

	float targetTime = speedFactor * currTime;
	std::vector<sKeyFrame> tempFrames;

	for (int i = 0; i < mpJointList.size(); i++)
	{
		float jointMaxTime = mpJointList[i].keyFramesByTake[0][take].back().keyTime;
		targetTime = std::fmod(targetTime, jointMaxTime);
		//Find the right keyframe based on time
		int currKeyIndex;
		const int frameCount = mpJointList[i].keyFramesByTake[0][take].size();
		//will pick the key with the time value that 
		//represents the "currTime" the best.
		int closestKey;
		float prevDiff = -1337;
		for (int j = 0; j < frameCount; j++)
		{
			if (prevDiff == -1337)
			{
				closestKey = j;
				prevDiff = abs(targetTime - mpJointList[i].keyFramesByTake[0][take][j].keyTime);
			}
			else {
				float currDiff = abs(targetTime - mpJointList[i].keyFramesByTake[0][take][j].keyTime);
				if (currDiff < prevDiff)
				{
					closestKey = j;
					prevDiff = currDiff;
				}
			}
			if (mpJointList[i].keyFramesByTake[0][take][j].keyTime == targetTime)
			{
				closestKey = j;
				break;
			}
			mpJointList[i].keyFramesByTake[0][take][j];
		}
		/**
		Now save this value in a "temporary final list of frames"
		**/
		tempFrames.push_back(mpJointList[i].keyFramesByTake[0][take][closestKey]);
	}
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

	//Find the root joint.
	int rootKey;
	for (int j = 0; j < mpJointList.size(); j++)
	{
		//If the joint has no parentjoint... That is: if it's the root.
		if (mpJointList[j].jointData->parentJointID < 0)
		{
			rootKey = j;
		}
	}

	/**
	Perform the requisite transforms on the root.
	We only care about the rotation.
	**/

	/**
	1. Get the key-transform for root.
	2. InverseBIndPose on it
	3. Multiply it with the input "transform-vector"
	**/
	float tempInvBindPose[16];
	for (int i = 0; i < 16; i++)
		tempInvBindPose[i] = mpJointList[rootKey].jointData->globalBindPoseInverse[i];
	
	glm::mat4 invBPose = convertToMat4(tempInvBindPose);

	//Now get the key-transform
	glm::mat4 keySMat = convertToScaleMat(tempFrames[rootKey].keyScale);
	glm::mat4 keyRMat = convertToRotMat(tempFrames[rootKey].keyRotate);
	glm::mat4 keyTMat = convertToTransMat(tempFrames[rootKey].keyPos);
	glm::mat4 keyTransform = keySMat * keyRMat * keyTMat;
	//Now "remove" the bindpose from the joint
	glm::mat4 pureKeyTransform = keyTransform * invBPose;
	//Now modify it with the worldMatrix
	glm::mat4 finalRootMatrix = pureKeyTransform * worldMat;
	
	//Saving the root matrix in the matrix array.
	jointMatrixList[rootKey] = finalRootMatrix;
	
	//Now loop through the root's children, and apply the root-matrix to their
	//"final transform". Then loop through the children's children and apply 
	//that "final transform" to them. Etc...
	recursiveUpdateJointMatrixList(finalRootMatrix, std::vector<sKeyFrame> tempFrames, rootKey);
}

bool Model::load( Assets* assets, std::string file )
{
	bool result = true;

	MoleReader assetData;
#if SLOW_LAUNCH
	assetData.readFromBinary( file.c_str() );
#else
	moleReader.readFromBinary( "Models/wallBox.mole" );
#endif

	/**
	Will now get all of the joints and their keyframes and their modelchildren into a single list, and order them based on jointID.
	I don't have to care TOO much about copying. I'll save memory-space anyway if I let go of the library-reader
	file. Don't have to keep hold of millions of vertices.
	**/

	const sMainHeader* mainHeader = assetData.getMainHeader();
	for (int i = 0; i < mainHeader->meshCount; i++)
	{
		sModelMesh tempMesh;
		const sMesh* currMesh = assetData.getMesh(i);
		const int jointCount = currMesh->jointCount;
		for (int j = 0; j < jointCount; j++)
		{
			sModelJoint tempJoint;
			const sJoint* currJoint = assetData.getJoint(i, j);

			for (int k = 0; k < currJoint->animationStateCount; k++)
			{
				const std::vector<sKeyFrame>* currFrameList = assetData.getKeyList(i, j, k);
				tempJoint.keyFramesByTake.push_back(currFrameList);
			}
			tempJoint.jointData = currJoint;
			tempJoint.meshChildren = assetData.getJointMeshChildList(i, j);
			mpJointList.push_back(tempJoint);
		}
		const std::vector<sMChildHolder>* meshChildListHolder = assetData.getMeshChildList();

		tempMesh.meshChildren = meshChildListHolder[0][i].meshChildList;
		tempMesh.meshData = currMesh;
		mpMeshList.push_back(tempMesh);
	}

	sortJointsByID();
	makeJointHierarchy();
	jointMatrixList.resize(mpJointList.size());
	
	mMeshes = assetData.getMeshList()->size();
	mMaps = assetData.getMaterialList()->size();

	mpMeshes = new Mesh[mMeshes];
	mpDiffuseMaps = new Texture*[mMaps];
	mpSpecularMaps = new Texture*[mMaps];
	mpNormalMaps = new Texture*[mMaps];

	for( int i=0; i<mMeshes && result; i++ )
		result = result && mpMeshes[i].load( &assetData, i );

for (int i = 0; i < mMaps && result; i++)
{
	const sMaterial* material = assetData.getMaterial(i);
	mpDiffuseMaps[i] = assets->load<Texture>(material->diffuseTexture);
	mpSpecularMaps[i] = assets->load<Texture>(material->specularTexture);
	mpNormalMaps[i] = assets->load<Texture>(material->normalTexture);

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

	mMeshes = mMaps = 0;

	// No need to unload the textures, Assets will take care of that
}

void Model::draw()
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
		if (mpJointList[i].jointData->jointID == searchID)
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
		if (mpJointList[i].jointData->parentJointID < 0)
			rootKey = i;
	}
	recursiveMakeJointHierarchy(rootKey);
}

void Model::recursiveMakeJointHierarchy(int parentID)
{
	for (int j = 0; j < mpJointList.size(); j++)
	{
		if (mpJointList[j].jointData->parentJointID == parentID)
		{
			//Add it as child.
			mpJointList[parentID].jointChildren.push_back(mpJointList[j].jointData->jointID);
			recursiveMakeJointHierarchy(j);
		}
	}
}

void Model::recursiveUpdateJointMatrixList(glm::mat4 parentTransformMatrix, std::vector<sKeyFrame> tempFrames, int currJointID)
{
	//The transform has to apply even when there's no children.
	mpJointList[currJointID].jointData;

	float tempInvBindPose[16];
	for (int i = 0; i < 16; i++)
		tempInvBindPose[i] = mpJointList[currJointID].jointData->globalBindPoseInverse[i];
	glm::mat4 invBPose = convertToMat4(tempInvBindPose);
	//Now get the key-transform
	glm::mat4 keySMat = convertToScaleMat(tempFrames[currJointID].keyScale);
	glm::mat4 keyRMat = convertToRotMat(tempFrames[currJointID].keyRotate);
	glm::mat4 keyTMat = convertToTransMat(tempFrames[currJointID].keyPos);
	glm::mat4 keyTransform = keySMat * keyRMat * keyTMat;
	//Now "remove" the bindpose from the joint
	glm::mat4 pureKeyTransform = keyTransform * invBPose;
	glm::mat4 finalTransform = pureKeyTransform * parentTransformMatrix;

	for (int i = 0; i < mpJointList[currJointID].jointChildren.size(); i++)
	{
		recursiveUpdateJointMatrixList(finalTransform, tempFrames, mpJointList[currJointID].jointChildren[i]);
	}
}

glm::mat4 Model::convertToTransMat(float inputArr[3])
{
	glm::mat4 output =
	{
		1.f, 0.f, 0.f, inputArr[0],
		0.f, 1.f, 0.f, inputArr[1],
		0.f, 0.f, 1.f, inputArr[2],
		0.f, 0.f, 0.f, 1.f
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

	mat4 rotMatrix = rotX * rotY * rotZ;
	
	return rotMatrix;
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
}