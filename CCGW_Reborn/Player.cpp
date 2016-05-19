#include "Player.h"
#include <iostream>
#define GLM_FORCE_RADIANS
#include "global_variables.h"
#include "Grid.h"
#include "glm\gtx\vector_angle.hpp"

glm::vec3 Player::getLookAt() const {
	return this->mLookat;
}

float Player::getRot() const
{
	return rotX;
}
float Player::getYOffset() {
	return this->yoffset;
}

void Player::update(const Input* inputs, const float &dt)
{
	GameObject::update( dt );

	mWorld = glm::mat4();
	bool canJump = false;

	if (inputs->buttonDown(0))
	{
		mStrength > 4 ? mStrength = 4 : mStrength += dt;
	
	}
		//mWeapon->update(dt);
	mWeapon.update( dt );
	speedY -= 25 * dt;
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
		for (int i = 0; i < pGameData->pGrid->getHeight();i++) {
			for (int j = 0; j < pGameData->pGrid->getWidth(); j++) {
				std::cout << (int)pGameData->pGrid->getTile(j, i) << " ";
			}
			std::cout << std::endl;
		}
		int a = 0;
		std::cout << std::endl;
	}

	if (mPosition.y - yoffset < 0.5) {
		mPosition.y = 0.0;
		speedY = 0;
		canJump = true;
	} 
	if (inputs->keyPressed(SDLK_SPACE) && canJump)
		speedY += 15;


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


	// #fuckedup maths, https://en.wikipedia.org/wiki/Rotation_matrix for formula
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
	mWorld = rotatematrix * glm::scale(mWorld, glm::vec3(0.1f, 0.1f, 0.1f));
	mWorld[3][0] = mPosition.x;
	mWorld[3][1] = mPosition.y;
	mWorld[3][2] = mPosition.z;
	mWorld[3][3] = 1.f;

	if (inputs->buttonReleased(0) )
	{
		if (mStrength > 0.25f)
		{
			glm::vec3 temp = { mLookat.x, 0, mLookat.z };
			glm::vec3 tempPos = this->mPosition - glm::vec3(0, 2*yoffset, 0) - glm::cross(glm::normalize(temp), glm::vec3(0, 1, 0)) / 4.f ;
			glm::vec3 la = glm::normalize((mPosition + 5.f*mLookat) - tempPos);
			float rotation = rotX - glm::angle(glm::normalize(glm::vec3(la.x, 0, la.z)), tempLookat);
			mWeapon.shoot(tempPos, la, rotation, mStrength);
		}
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

//void Player::render(const GLuint & programID, const glm::mat4 &viewMat)

void Player::render( GLuint worldLocation, GLuint animationLocation )
{
	GameObject::renderAni(worldLocation, animationLocation);
}
void Player::renderArrows(GLuint worldLocation) {
	this->mWeapon.render(worldLocation);
}

void Player::setAlive(bool amIalive)
{
	mHealth = 25;
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
			int x = (int)((int)(coord.x + 1.0f) / pGameData->boxScale) + i;
			int y = (int)((int)(coord.z + 1.0f) / pGameData->boxScale) + j;
			if (!(y < 0 || x < 0 || y >= (int)pGameData->pGrid->getHeight() || x >= (int)pGameData->pGrid->getWidth())) {
				uchar b = pGameData->pGrid->getTile(x, y);
				if (b != TILE_BLOCKED && b != TILE_EMPTY) {
					if (this->mBB.intersect(glm::vec3(x*pGameData->boxScale, 1, y*pGameData->boxScale), pGameData->boxScale / 2)) {
						intersect = true;
					}
				}
			}
		}
	}

	if( !intersect )
	{
		int w = pGameData->pGrid->getWidth();
		int h = pGameData->pGrid->getHeight();

		intersect = ( coord.x < 0.0f || coord.x >= (w-1)*pGameData->boxScale || coord.z < 0.0f || coord.z >= (h-1)*pGameData->boxScale);
	}
	return !intersect;
}

void Player::takeDamage(int damage) 
{
	this->mHealth -= damage;
}

bool Player::isAlive() {
	return this->mHealth > 0;
}

int Player::getHealth() const
{
	return mHealth;
}

Player::Player() 
{}

Player::Player(GameData* data, Emitter* emitter) : GameObject()
{
	this->pGameData = data;
	//mWeapon = new Weapon(true, data);
	mWeapon.load( data, true, emitter);
	mPosition = glm::vec3( 1, 1, 1 );
	yoffset = -0.5f;
	mWorld = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, yoffset, 1, 1 };
	mMaxSpeed = 10;
	speedY = 0;
	rotX = glm::pi<float>() * -0.5f;
	mStrength = 0.0f;
	mHealth = 90;
	daIndex = 0;
	//setScale( 0.1f );
}

Player::~Player()
{
	//delete mWeapon;
}