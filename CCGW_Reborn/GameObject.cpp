#include "GameObject.h"
#include <iostream>

bool GameObject::load( Model* model )
{
	mpModel = model;
	return true;
}

void GameObject::update(const float &dt) {
}

void GameObject::render(const GLuint & programID)
{
	GLuint world = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(world, 1, GL_FALSE, &this->mWorld[0][0]);
	mpModel->draw();
}

void GameObject::render(const GLuint & programID, const glm::mat4 &viewMat)
{
	GLuint world = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(world, 1, GL_FALSE, &this->mWorld[0][0]);
	mpModel->draw();
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

void GameObject::loadSound(Sound* sound)
{
	if (!sound)
	{
		const char* error = Mix_GetError();
		std::cout << "Mixer error: " << error << std::endl;
	}
	else
		this->mSound = sound;
	int x = 0;
}

GameObject::GameObject(const GameObject& ref)
	: mWorld(ref.mWorld), rotX(0),rotY(0), mpModel( ref.mpModel )
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
}

GameObject::GameObject()
	:scale(1.0f), rotX(0), rotY(0), mpModel( nullptr )
{
	mPosition = { 0, 0, 0 };
	mLookat = { 0, 0, -1 };
	mWorld = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	this->mBB = BoundingBox(mPosition, scale);
}

GameObject::~GameObject()
{

}