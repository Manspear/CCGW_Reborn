#pragma once
#include <glm\glm.hpp>
#include <vector>
class Joint 
{
public:

	struct sKeyData
	{
		glm::mat4x4 keyTransform;
		float keyTime;
	};
	/**
	The keys implicitly know which joint they belong to.
	**/
	struct sJointData
	{
		const char* name;
		unsigned int parentJointIndex;
		unsigned int jointIndex;
		glm::mat4x4 globalBindPoseInverse;
		std::vector<sKeyData> setKeys;
	};

	sJointData mJointData;

	Joint();
	~Joint();
};