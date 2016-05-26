#pragma once

#include <GLM\glm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Texture.h"
#include "Input.h"
#include "GameData.h"
#include "global_variables.h"
#include "BoundingBox.h"
#include "Model.h"
#include "Sound.h"
#include "Animation.h"

class GameObject
{
public:
	//Gold gets "subtracted" in marker.cpp
	#define BALLISTACOST 30
	#define BOXCOST 1

	bool load( Model* model );
	virtual void update(const float &dt);
	virtual void updateAnimation();
	virtual void renderAni( GLuint worldLocation, GLuint animationLocation );
	virtual void renderNonAni( GLuint worldLocation, GLuint tintLocation );
	glm::vec3 getPosition() const;
	void setPosition( glm::vec3 position );
	void setScale(float scale);
	void setWorld(glm::mat4 world);
	void playAnimation( int animation, bool looping, float speed = 1.0f );
	void loadSound(Sound* sound);
	void playSound();
	virtual void randomizeAnimation();

	GameObject(const GameObject& ref);
	GameObject(glm::vec3 position, float scale);
	GameObject();
	virtual ~GameObject();
protected: // <-- changed to protected from private. because i wanted to reach these from subclasses.
	glm::mat4 mWorld;
	glm::vec3 mPosition;
	glm::vec3 mLookat;
	float rotX;
	float rotY;
	float scale;
	Sound* mSound;
	//float animationTime;
	//int mAnimation;

	std::vector<glm::mat4> mJointMatrixList;

	Animator mAnimator;

	BoundingBox mBB;

	Model* mpModel;
};