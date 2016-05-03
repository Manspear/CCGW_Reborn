#include "Tower.h"
#define GLM_FORCE_RADIANS
#include <glm\gtx\vector_angle.hpp>


void Tower::update(GameData* gameData, const float & dt)
{
	if (mWeaponReady)
	{
		for (int i = 0; i < gameData->mMoleratmen; i++) {
			if (gameData->pMoleratmen[i].getAlive()) {
				glm::vec3 direction = gameData->pMoleratmen[i].getPosition() - mPosition;
				mDistanceToTarget = glm::length(direction);
				direction = glm::normalize(direction);
				if (mDistanceToTarget < mRange)
				{
					mWeaponReady = false;
					mShooting = true;
					mReloadTime = 3;
					targetEnemy = &gameData->pMoleratmen[i];
					mpWeapon->shoot(mPosition, direction, rotX, mStrength);
					break;
				}
			}
		}
	}
	else {
		mReloadTime -= mFireRate * dt;
		if (mReloadTime < 0.0)
			mWeaponReady = true;
	}
	if (mShooting) {
		mpWeapon->update(dt);
		mShooting = arrowShot(dt, gameData);
	}
}

bool Tower::arrowShot(const float &dt, GameData* data) {
	bool targetHit = true;
	mDistanceToTarget -= mStrength * 15 * dt;
	if (mDistanceToTarget <= 0.0f)
	{
		targetHit = false;	
		if (targetEnemy->getAlive())
		{
			targetEnemy->imHit(mStrength);
			if (!targetEnemy->getAlive())
			{
				data->pGold++;
				std::cout << "du fick peng " << data->pGold << std::endl;
				data->pScore++;
			}
		}
	}
	return targetHit;
}
void Tower::render(const GLuint & programID)
{
	GameObject::render(programID);
	if (mShooting)
		mpWeapon->draw(programID);
}

Tower::Tower(GameData* gameData, glm::vec3 position, const Tower &towerRef, float scale): GameObject(position, scale)
{
	this->mpMesh = towerRef.mpMesh;
	this->mpTexture = towerRef.mpTexture;
	this->mpSpecularMap = towerRef.mpSpecularMap;
	this->mpNormalMap = towerRef.mpNormalMap;
	this->mpWeapon = new Weapon(false, gameData);
	this->mWeaponReady = true;
	mLookat = { 1 ,0, 0 };
	mReloadTime = 5;
	mFireRate = 3;
	mShooting = false;
	mRange = 10;
	mStrength = 2;
}

Tower::Tower()
{
	mpWeapon = nullptr;
}


Tower::~Tower()
{
	delete mpWeapon;
}
