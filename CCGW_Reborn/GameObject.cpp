#include "GameObject.h"
#include <iostream>
#include <glm\gtc\type_ptr.hpp>

bool GameObject::load( Model* model )
{
	mpModel = model;
	mAnimator.setModel( model );
	return true;
}

void GameObject::update(const float &dt) {
	mAnimator.update(dt);
}

void GameObject::updateAnimation() {
	mpModel->updateAnimation(1.0f, mAnimator.getCurrentAnimation()->mTake, mAnimator.getElapsed() );
}

//void GameObject::renderAni(const GLuint & programID)
void GameObject::renderAni( GLuint worldLocation, GLuint animationLocation )
{
    //mpModel->updateAnimation(1.f, mAnimation, animationTime, mWorld);
	
	updateAnimation();

	//animationTime += 0.01f;

	//GLuint world = glGetUniformLocation(programID, "animationMatrices");
	glUniformMatrix4fv(animationLocation, mpModel->jointMatrixList.size(), GL_FALSE, glm::value_ptr(mpModel->jointMatrixList[0]));
	//GLuint world2 = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(worldLocation,1, GL_FALSE, &mWorld[0][0]);
	mpModel->drawAni();
}

//void GameObject::renderNonAni(const GLuint & programID)
void GameObject::renderNonAni( GLuint worldLocation )
{
	//GLuint world2 = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &mWorld[0][0]);
	mpModel->drawNonAni();
}

glm::vec3 GameObject::getPosition() const
{
	return mPosition;
}

void GameObject::setPosition( glm::vec3 position )
{
	mPosition = position;
	mWorld[3][0] = position.x;
	mWorld[3][1] = position.y;
	mWorld[3][2] = position.z;
}

void GameObject::setScale(float scale)
{
	mWorld[0][0] = scale;
	mWorld[1][1] = scale;
	mWorld[2][2] = scale;
}

void GameObject::setWorld(glm::mat4 world)
{
	mWorld = world;
}

void GameObject::playAnimation( int animation, bool looping, float speed )
{
	//mAnimation = animation;
	mAnimator.push( animation, looping, speed );
}

void GameObject::loadSound(Sound* sound)
{
	mSound = sound;
}

GameObject::GameObject(const GameObject& ref)
	: mWorld(ref.mWorld), rotX(0),rotY(0), mpModel( ref.mpModel )//, mAnimation( ref.mAnimation)
{

}

GameObject::GameObject(glm::vec3 position = { 0, 0, 0 }, float scale = 1.0f)
{
	mPosition = position;
	this->mBB = BoundingBox(mPosition, scale);
	mWorld = {
				scale,			0,				0,				0,
				0,				scale,			0,				0,
				0,				0,				scale,			0,
				position.x,		position.y,		position.z,		1.0 
	};
	mSound = nullptr;
	/*animationTime = 0;
	mAnimation = 0;*/
}

GameObject::GameObject()
	:scale(1.0f), rotX(0), rotY(0), mpModel( nullptr )
{
	mPosition = { 0, 0, 0 };
	mLookat = { 0, 0, -1 };
	mWorld = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	this->mBB = BoundingBox(mPosition, scale);
	/*animationTime = 0;
	mAnimation = 0;*/
}

GameObject::~GameObject()
{

}

void GameObject::playSound() {
	this->mSound->play();
}