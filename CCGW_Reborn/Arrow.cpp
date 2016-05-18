#include "Arrow.h"
#define GLM_FORCE_RADIANS
#include "glm\gtx\vector_angle.hpp"
#include "Moleratman.h"
#include "Molebat.h"
#include "Tower.h"
 
bool Arrow::load(GameData* data, Model* model, Emitter* emitter )
{
	bool result = true;
	mpModel = model;
	mEmitter = *emitter;
	pGameData = data;
	return result;
}

bool Arrow::isAlive() 
{
	return mAlive;
}
void Arrow::update(float dt)
{
	this->mTimeSinceLastEmmit += dt;
	glm::vec3 lastPos = mPosition;
	mVelocity += mGravitation * 0.05f;
	mPosition += mVelocity * dt;

	mLookat = glm::normalize( mPosition - lastPos );
	glm::vec3 tempVec = glm::normalize( glm::vec3( mLookat.x, 0, mLookat.z ) );
	rotY = glm::angle( mLookat, tempVec );
	
	float p = glm::pi<float>() * 0.5f;

	this->mWorld =	glm::mat4(
								cosf(rotY)* cosf(rotX+p),		sinf(rotY),			cosf(rotY) * sinf(rotX+p),			0,
								-sinf(rotY) * cosf(rotX+p),		cosf(rotY),			-sinf(rotY) * sinf(rotX+p),			0,
								-sinf(rotX+p),					0,					cosf(rotX+p),						0,
								mPosition.x,					mPosition.y,		mPosition.z,						1
	);


	if (mTimeSinceLastEmmit > mEmmitInterval)
	{
		mEmitter.spawn(mPosition, glm::vec3(0.0f, 0.1f, 0.0f), 2.f, 0.1f, glm::vec2(0.3f), glm::vec2(0.1f));
		mTimeSinceLastEmmit = 0;
	}

	if (mPlayerOwned)
	{
		for (int i = 0; i < pGameData->mMoleratmen && mAlive; i++)
		{
			if (pGameData->pMoleratmen[i].getAlive())
			{
				bool unique = true;
				for( int i=0; i<mPiercings && unique; i++ )
				{
					if( mpPiercedEnemies[i] == &pGameData->pMoleratmen[i] )
						unique = false;
				}

				if( unique )
				{
					float damage = 0.0f;
					// check headshot
					if (pGameData->pMoleratmen[i].getHeadBox().intersect(lastPos, mPosition))
						damage = mDamage * 2.0f;
					// check bodyshot
					else if (pGameData->pMoleratmen[i].getBoundingBox().intersect(lastPos, mPosition))
						damage = mDamage;

					if (damage > 0.0f)
					{
						pGameData->pMoleratmen[i].imHit(damage, mPosition);

						if (!pGameData->pMoleratmen[i].getAlive()) {
							pGameData->pGold++;
							pGameData->pScore++;
						}

						if (mPiercing)
						{
							mpPiercedEnemies[mPiercings++] = &pGameData->pMoleratmen[i];
							if( mPiercings >= ARROW_MAX_PIERCING_DEPTH )
								mAlive = false;
						}
						else
							mAlive = false;
					}
				}
			}
		}

		// check collision against molebats
		for (int i = 0; i < pGameData->mMolebats && mAlive; i++)
		{
			if (pGameData->pMolebats[i].getAlive())
			{
				bool unique = true;
				for( int i=0; i<mPiercings; i++ )
				{
					if( mpPiercedEnemies[i] == &pGameData->pMolebats[i] )
						unique = false;
				}

				if( unique )
				{
					float damage = 0.0f;
					if (pGameData->pMolebats[i].getHeadBox().intersect(lastPos, mPosition))
						damage = mDamage * 2.0f;
					else if (pGameData->pMolebats[i].getBoundingBox().intersect(lastPos, mPosition))
						damage = mDamage;

					if (damage > 0.0f)
					{
						pGameData->pMolebats[i].imHit(damage, mPosition);
						//mEmitter.spawn(mPosition, glm::vec3(0.0f, -1.0f, 0.0f), 1.0f);
						if (!pGameData->pMolebats[i].getAlive()) {
							pGameData->pGold++;
							pGameData->pScore++;
						}
						if (mPiercing)
						{
							mpPiercedEnemies[mPiercings++] = &pGameData->pMolebats[i];
							if( mPiercings >= ARROW_MAX_PIERCING_DEPTH )
								mAlive = false;
						}
						else
							mAlive = false;
					}
				}
			}
		}

		// check collision against tower
		int x = (int)((int)(mPosition.x + 1.0f) / pGameData->boxScale);
		int y = (int)((int)(mPosition.z + 1.0f) / pGameData->boxScale);
		if (!(y < 0 || x < 0 || y >= (int)pGameData->pGrid->getHeight() || x >= (int)pGameData->pGrid->getWidth())) {
			uchar b = pGameData->pGrid->getTile(x, y);
			if (b != TILE_EMPTY && b != TILE_BLOCKED) {
				if( mPosition.y < pGameData->boxScale ){
					mAlive = false;
				}
			}
		}
	}
	// check if we hit the ground
	if( mPosition.y < 0 )
		mAlive = false;
}
Arrow::Arrow() : GameObject({0,-10,0}, 1.0f)
{
	rotX = 0;
	mEmmitInterval = 0.1;
	mTimeSinceLastEmmit = 0;
	mLookat = {1,0,0};
	mSpeed = 1.f;
	mGravitation = {0,-1,0};
	mpModel = nullptr;
	mSound = nullptr;
	//mpSpecularMap = nullptr;
	//mpNormalMap = nullptr;
	pGameData = nullptr;
	mAlive = false;
	mPlayerOwned = false;
	mPiercing = true;

	mPiercings = 0;
	for (int i = 0; i<ARROW_MAX_PIERCING_DEPTH; i++)
		mpPiercedEnemies[i] = nullptr;
}

void Arrow::spawn(bool owner, glm::vec3 position, glm::vec3 direction, float travelSpeed, glm::vec3 downVector, float rotation, float damage)
{
	mPlayerOwned = owner;
	rotY = 0;
	rotX = rotation;
	mPosition = position;
	mLookat = direction;
	mSpeed = travelSpeed;
	mGravitation = downVector;
	mDamage = damage;

	mVelocity = direction * travelSpeed;
	mAlive = true;
	mPiercing = true;
	if (mSound != nullptr)
		mSound->play();
	mPiercings = 0;
	for (int i = 0; i<ARROW_MAX_PIERCING_DEPTH; i++)
		mpPiercedEnemies[i] = nullptr;
	
}
Arrow::~Arrow() 
{
	
}
