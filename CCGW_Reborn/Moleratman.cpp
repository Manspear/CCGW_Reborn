#include "Moleratman.h"
#include "Player.h"

void Moleratman::update(float dt, GameData* data)
{
	Enemy::update( dt );

	if (mCurrent >= 0)
	{
		glm::vec3 target(pPath[mCurrent].x * pGameData->boxScale, 0.0f, pPath[mCurrent].y * pGameData->boxScale);

		float dist = glm::distance(mPosition, glm::vec3(target));
		if (dist < MOLERATMAN_SPEED * dt)
		{
			mCurrent--;
			glm::vec3 dir = glm::normalize( target - mPosition );
			mPosition = target;

			target = mPosition + dir;
		}
		else
		{
			glm::vec3 dir = glm::normalize(target - mPosition);
			mPosition += dir * MOLERATMAN_SPEED * dt;
		}

		mWorld[3][0] = mPosition.x;
		mWorld[3][1] = mPosition.y;
		mWorld[3][2] = mPosition.z;

		// face the direction of movement
		glm::vec3 dir = glm::normalize( target - mPosition );

		mLookat = glm::normalize(glm::vec3(dir.x, 0.0f, dir.z));
		rotY = -glm::angle(mLookat, glm::vec3(1.0f, 0.0f, 0.0f));
		if (mLookat.z < 0.0f)
			rotY *= -1.0f;		
	}
	else if (mAlive && mCurrent <= 0){
		if (--data->mBabyCount <= 0)
			data->pPlayer->takeDamage(1000);
		mAlive = false;

	}
	if (mLife <= 0) {
		mAlive = false;
	}

		float p = glm::pi<float>()*1.5f;
		rotY -= p;

		mWorld = {
			cosf(rotY)/2,	0,		-sinf(rotY) / 2,	0,
			0,				1.0f / 2.0f,		0,				0,
			sinf(rotY) / 2,		0,		cosf(rotY) / 2,		0,
			mPosition.x,	mPosition.y+1,	mPosition.z, 1
		};
	mBoundingBox.center = mPosition + glm::vec3( 0, 0.75f, 0 );

	glm::vec3 headOffset = mLookat*0.5f;
	headOffset.y = 1.5f;
	mHeadBox.center = mPosition + headOffset;
}

void Moleratman::imHit( float strength, glm::vec3 position )
{
	Enemy::imHit( strength, position );

	mAnimator.push( 2, false, 5.0f );
}

Moleratman& Moleratman::operator=( const Moleratman& ref )
{
	Enemy::operator=( ref );
	return *this;
}

Moleratman::Moleratman( const Moleratman& ref )
	: Enemy( ref )
{
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Moleratman::Moleratman()
{
	mSound = nullptr;
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Moleratman::~Moleratman()
{
}