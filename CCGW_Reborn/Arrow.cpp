#include "Arrow.h"
#define GLM_FORCE_RADIANS
#include "glm\gtx\vector_angle.hpp"
#include "Moleratman.h"
#include "Molebat.h"
#include "Tower.h"
 
bool Arrow::load(GameData* data, Model* model, Emitter* emitter )
{
	bool result = true;
	//mpMesh = data->pAssets->load<tempMesh>(mesh);
	mpModel = model;

	//mpEmitter.load( data, "Models/pns.png" );
	//data->mEmitters.push_back( &mpEmitter );
	
	//pEmitter = data->pEmitter;

	/*if( data->pEmission->allocEmitter( &mEmitter, 50 ) )
		result = result && mEmitter.load( data, "Models/pns.png" );
	else
		result = false;*/

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

	/*glm::vec3 tempVec = glm::normalize(glm::vec3(mLookat.x, 0, mLookat.z));
	this->rotY = glm::angle(mLookat, tempVec);
	if (mLookat.y < 0) {
		rotY *= -1;
	}
	this->mPosition += mSpeed*mLookat*dt;
	this->mLookat = glm::normalize((this->mLookat*this->mSpeed) + 5.0f*mGravitation*dt);*/


	glm::vec3 lastPos = mPosition;
	mPosition += mVelocity * dt;
	mVelocity += mGravitation * 0.05f;

	//rotY = atan2( lastPos.y - mPosition.y, lastPos.x - mPosition.x );
	mLookat = glm::normalize( mPosition - lastPos );
	glm::vec3 tempVec = glm::normalize( glm::vec3( mLookat.x, 0, mLookat.z ) );
	rotY = glm::angle( mLookat, tempVec );
	if( mLookat.y < 0 )
		rotY *= -1;

	/*
	glm::mat3 rotationY = glm::mat3(
		cosf(rotX), 0, sinf(rotX),
		0, 1, 0,
		-sinf(rotX), 0, cosf(rotX));
	glm::mat3 rotationX = glm::mat3(1, 0, 0,
		0, cosf(rotY), sinf(rotY),
		0, -sinf(rotY), cosf(rotY));

	glm::mat3 totalRotation = rotationX * rotationY;
	this->mWorld = glm::mat4(glm::vec4(totalRotation[0], 0),
		glm::vec4(totalRotation[1], 0),
		glm::vec4(totalRotation[2], 0),
		glm::vec4(mPosition.x, mPosition.y, mPosition.z, 1));
	*/
	/*
	glm::mat3 rotationY = glm::mat3(
		cosf(rotY), 0, sinf(rotY),
		0, 1, 0,
		-sinf(rotY), 0, cosf(rotY));
	glm::mat3 rotationX = glm::mat3(1,0,0,
									0,	cosf(rotX - glm::pi<float>()*0.5f), sinf(rotX - glm::pi<float>()*0.5f),
									0,	-sinf(rotX - glm::pi<float>()*0.5f), cosf(rotX - glm::pi<float>()*0.5f));

	glm::mat3 totalRotation = rotationY * rotationX;
	this->mWorld = glm::mat4(glm::vec4(totalRotation[0], 0),
		glm::vec4(totalRotation[1], 0),
		glm::vec4(totalRotation[2], 0),
		glm::vec4(mPosition.x, mPosition.y, mPosition.z, 1));
	*/
	/*
	this->mWorld = {
		cosf(rotX),				sinf(rotY)*sinf(rotX),		sinf(rotX),				0,
		0,						cosf(rotX),					sinf(rotX),				0,
		sinf(rotX),				cosf(rot)*sinf(rotX)*-1.f,	cosf(rotY)*cosf(rotX),	0,
		mPosition.x,			mPosition.y,				mPosition.z,			1
	};*/
	
	float p = glm::pi<float>() * 0.5f;
	this->mWorld = {
					cosf(rotY)* cosf(rotX-p),		sinf(rotY),			cosf(rotY) * sinf(rotX-p),			0,
					-sinf(rotY) * cosf(rotX-p),		cosf(rotY),			-sinf(rotY) * sinf(rotX-p),			0,
					-sinf(rotX-p),					0,					cosf(rotX-p),						0,
					mPosition.x,					mPosition.y,		mPosition.z,						1
	};

	if (mTimeSinceLastEmmit > mEmmitInterval)
	{
		mEmitter.spawn(mPosition, glm::vec3(0.0f, 0.1f, 0.0f), 2.f, 0.1f, glm::vec2(0.3f), glm::vec2(0.1f));
		mTimeSinceLastEmmit = 0;
	}

	// check collision against moleratmen
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
						damage = mSpeed * 2.0f;
					// check bodyshot
					else if (pGameData->pMoleratmen[i].getBoundingBox().intersect(lastPos, mPosition))
						damage = mSpeed;

					if (damage > 0.0f)
					{
						pGameData->pMoleratmen[i].imHit(damage);

						mEmitter.spawn(mPosition, glm::vec3(0.0f, -1.0f, 0.0f), 1.0f);
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
						damage = mSpeed * 2.0f;
					else if (pGameData->pMolebats[i].getBoundingBox().intersect(lastPos, mPosition))
						damage = mSpeed;

					if (damage > 0.0f)
					{
						pGameData->pMolebats[i].imHit(damage);
						mEmitter.spawn(mPosition, glm::vec3(0.0f, -1.0f, 0.0f), 1.0f);
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
			if ( b != TILE_EMPTY && b!=TILE_BLOCKED) {
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

void Arrow::spawn(bool owner, glm::vec3 position, glm::vec3 direction, float travelSpeed, glm::vec3 downVector, float rotation)
{
	mPlayerOwned = owner;
	rotY = 0;
	rotX = rotation;
	mPosition = position;
	mLookat = direction;
	mSpeed = travelSpeed;
	mGravitation = downVector;

	mVelocity = direction * travelSpeed;
	mAlive = true;
	mPiercing = true;

	mPiercings = 0;
	for (int i = 0; i<ARROW_MAX_PIERCING_DEPTH; i++)
		mpPiercedEnemies[i] = nullptr;
	
}
Arrow::~Arrow() 
{
	
}
