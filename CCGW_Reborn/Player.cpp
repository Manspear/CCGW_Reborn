#include "Player.h"
#include <iostream>
#include "global_variables.h"
#include "Grid.h"

glm::vec3 Player::getLookAt() const {
	return this->mLookat;
}

float Player::getRot() const
{
	return rotX;
}

void Player::update(const Input* inputs, const float &dt)
{
	bool canJump = false;

	if (inputs->buttonDown(0))
		this->mStrength += dt;
	mWeapon->update(dt);
	speedY -= 15 * dt;
	mSpeed *= 1- 15*dt;
	if (mSpeed < 0)
		mSpeed = 0;
	//mSpeed = 0;
	glm::vec3 tempLookat = glm::normalize(glm::vec3(mLookat.x, 0, mLookat.z));
	glm::vec3 dir(0.0f, 0.0f, 0.0f);

	if (inputs->keyDown(SDLK_w))
	{
		mSpeed = mMaxSpeed;
		dir += glm::vec3(cos(rotX), 0.0f, sin(rotX));
	}
	if (inputs->keyDown(SDLK_s))
	{
		mSpeed = mMaxSpeed;
		float r = glm::pi<float>();
		dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	}
	if (inputs->keyDown(SDLK_a))
	{
		mSpeed = mMaxSpeed;
		float r = glm::pi<float>() * 0.5f;
		dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	}
	if (inputs->keyDown(SDLK_d))
	{
		mSpeed = mMaxSpeed;
		float r = glm::pi<float>() * 1.5f;
		dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	}
	if (glm::length(dir) > 0.1f)
	{
		dir = glm::normalize(dir);
		mDirection = dir;
	}

	float dx = mDirection.x*mSpeed*dt;
	float dz = mDirection.z*mSpeed*dt;
	float dy = speedY*dt;
	
	glm::vec3 deltaPos = {0,0,0};
	if (checkMove({mPosition.x + dx,mPosition.y,mPosition.z }))
		deltaPos.x = dx;
	if (checkMove({ mPosition.x,mPosition.y + dy,mPosition.z }))
		deltaPos.y = dy;
	else {
		speedY = 0;
		canJump = true;
	}
	if (checkMove({ mPosition.x,mPosition.y,mPosition.z + dz }))
		deltaPos.z = dz;
	mPosition += deltaPos;

	if (inputs->keyPressed(SDLK_q)) {
		for (int i = 0; i < mGameData->pGrid->getHeight();i++) {
			for (int j = 0; j < mGameData->pGrid->getWidth(); j++) {
				std::cout << (int)mGameData->pGrid->getTile(j, i) << " ";
			}
			std::cout << std::endl;
		}
		int a = 0;
		std::cout << std::endl;
	}

	if (mPosition.y < 0.5) {
		mPosition.y = 0.5;
		speedY = 0;
		canJump = true;
	}

	if (inputs->keyPressed(SDLK_SPACE) && canJump)
		speedY += 10;


	double degree = (inputs->mouseDelta().x) / 200 * -1;
	double rad = (inputs->mouseDelta().y) / 400 * -1;

	rotX += degree;

	if (rotX > glm::pi<float>()*2.0f)
		rotX -= glm::pi<float>()*2.0f;
	else if (rotX < 0.0f)
		rotX += glm::pi<float>()*2.0;

	glm::mat4 rotatematrix = { cosf(degree), 0, sinf(degree),0,
								0,1,0,0,
								-sinf(degree), 0, cosf(degree),0,
								0,0,0,1
	};


	// #fuckedup maths, https://en.wikipedia.org/wiki/Rotation_matrix for formula, might gimbalock??(but probably not)
	glm::vec3 axis = glm::normalize(glm::cross(tempLookat, glm::vec3(0, 1, 0)));
	glm::mat3 ucm = { 0,-axis.z, axis.y,
						axis.z, 0, -axis.x,
						-axis.y, axis.x, 0 };
	glm::mat3 tensorprodU = { axis.x*axis.x, axis.x*axis.y, axis.x*axis.z,
								axis.y*axis.x, axis.y*axis.y, axis.y*axis.z,
								axis.z*axis.x, axis.z*axis.y, axis.z*axis.z };

	glm::mat3 tempMatrix = cosf(rad)* glm::mat3() + sinf(rad)*ucm + (1 - cosf(rad))*tensorprodU;
	glm::mat4 rotateAroundZ = glm::mat4(
		glm::vec4(tempMatrix[0], 0),
		glm::vec4(tempMatrix[1], 0),
		glm::vec4(tempMatrix[2], 0),
		glm::vec4(0, 0, 0, 1)
	);

	if (glm::dot(glm::vec3(rotateAroundZ * glm::vec4(mLookat, 1)), tempLookat) > 0) {
		this->mLookat = glm::vec3(rotateAroundZ * glm::vec4(mLookat, 1));
	}

	//mLookat = glm::rotate(glm::quat(cosf(mRot),0,sinf(mRot),0), glm::vec3(0,1));

	//this->mRotation.z = 
	this->mLookat = glm::vec3(rotatematrix * glm::vec4(mLookat, 1));
	mWorld = rotatematrix * mWorld;

	mWorld[3][0] = mPosition.x;
	mWorld[3][1] = mPosition.y;
	mWorld[3][2] = mPosition.z;
	mWorld[3][3] = 1.f;

	if (inputs->buttonReleased(0)) {
		mWeapon->shoot(this->mPosition, mLookat, rotX, mStrength);
		mStrength = 0;
	}
}
glm::vec3 Player::tacticalUpdate(const Input * inputs, const float &dt, const GameData &gameData)
{
	glm::vec3 dir(0.0f, 0.0f, 0.0f);	
	if (inputs->keyDown(SDLK_w))
	{
		dir += glm::vec3(0.0f, 0.0f, -10.0 * dt);
	}
	if (inputs->keyDown(SDLK_s))
	{
		dir += glm::vec3(0.0f, 0.0f, 10.0 * dt);
	}
	if (inputs->keyDown(SDLK_a))
	{
		dir += glm::vec3(-10.0 * dt, 0.0f, 0.0f);
	}
	if (inputs->keyDown(SDLK_d))
	{
		dir += glm::vec3(10.0 * dt, 0.0f, 0.0f);
	}
	return dir;
}

void Player::render(const GLuint & programID, const glm::mat4 &viewMat)
{
	GameObject::render(programID, viewMat);
	this->mWeapon->draw(programID);
}

glm::vec3 Player::getMovingDirection(glm::vec3 v1, glm::vec3 v2) {
	glm::vec3 result = glm::normalize(v1 + v2);
	if (result != result)
 		result = glm::vec3(0,0,0);
	return result;
}
bool Player::checkMove(glm::vec3 coord) {
	mBB.center = coord;

	bool intersect = false;
	for (int i = -1; i <= 1 && !intersect; i++) {
		for (int j = -1; j <= 1 && !intersect; j++) {
			int x = (int)((int)(coord.x + 1.0f) / mGameData->boxScale) + i;
			int y = (int)((int)(coord.z + 1.0f) / mGameData->boxScale) + j;
			if (!(y < 0 || x < 0 || y >= (int)mGameData->pGrid->getHeight() || x >= (int)mGameData->pGrid->getWidth())) {
				if (mGameData->pGrid->getTile(x, y) != TILE_EMPTY) {
					if (this->mBB.intersect(glm::vec3(x*mGameData->boxScale, 1, y*mGameData->boxScale), mGameData->boxScale / 2)) {
						intersect = true;
					}
				}
			}
		}
	}
	return !intersect;
}

Player::Player() 
{}

Player::Player(GameData* data) : GameObject()
{
	this->mGameData = data;
	mWeapon = new Weapon(data);
	mWorld = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	mMaxSpeed = 10;
	speedY = 0;
	rotX = glm::pi<float>() * -0.5f;
	mStrength = 0.0f;
}

Player::~Player()
{
	delete mWeapon;
}