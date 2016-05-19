#include "Moleratman.h"
#include "Player.h"

void Moleratman::update(float dt, GameData* data)
{
	//Enemy::update( dt );

	if (mCurrent >= 0)
	{
		glm::vec3 target(pPath[mCurrent].x * pGameData->boxScale, 0.0f, pPath[mCurrent].y * pGameData->boxScale);

		float dist = glm::distance(mPosition, glm::vec3(target));
		if (dist < mSpeed * dt)
		{
			sNode prev = pPath[mCurrent];

			mCurrent--;

			sNode cur = pPath[mCurrent];

			sNode dif = { cur.x-prev.x, cur.y-prev.y };
			if( dif.x != mPrevTargetDif.x || dif.y != mPrevTargetDif.y )
			{
					if( mPrevTargetDif.y != 0 )
					{
						int x = dif.x*mPrevTargetDif.y;
						if( x < 0 )
						{
							if( mAnimator.getCurrentTake() != ANIM_TURN_RIGHT)
							{
								mAnimator.push(ANIM_TURN_RIGHT, false, 2.0f, 0.9f );
							}
						}
						else
						{
							if( mAnimator.getCurrentTake() != ANIM_TURN_LEFT)
							{
								mAnimator.push(ANIM_TURN_LEFT, false, 2.0f, 0.9f);
							}
						}
					}
					else
					{
						int y = dif.y*mPrevTargetDif.x;
						if( y > 0 )
						{
							if( mAnimator.getCurrentTake() != ANIM_TURN_RIGHT )
							{
								mAnimator.push(ANIM_TURN_RIGHT, false, 2.0f, 0.9f);
							}
						}
						else
						{
							if( mAnimator.getCurrentTake() != ANIM_TURN_LEFT )
							{
								mAnimator.push(ANIM_TURN_LEFT, false, 2.0f, 0.9f);
							}
						}
					}
			}

			mPrevTargetDif = dif;

			glm::vec3 dir = glm::normalize( target - mPosition );
			mPosition = target;

			target = mPosition + dir;
		}
		else
		{
			glm::vec3 dir = glm::normalize(target - mPosition);
			mPosition += dir * mSpeed * dt;
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
		data->pBabies[0].playSound();
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
			mPosition.x,	mPosition.y+1.35f,	mPosition.z, 1
		};
	mBoundingBox.center = mPosition + glm::vec3( 0, 1.0f, 0 );

	glm::vec3 headOffset = mLookat*0.75f;
	headOffset.y = 2.0f;
	mHeadBox.center = mPosition + headOffset;

	Enemy::update(dt);
}

void Moleratman::imHit( float strength, glm::vec3 position )
{
	Enemy::imHit( strength, position );

	mAnimator.push( ANIM_STAGGER, false, 5.0f );
}

Moleratman& Moleratman::operator=( const Moleratman& ref )
{
	Enemy::operator=( ref );
	return *this;
}

Moleratman::Moleratman( const Moleratman& ref )
	: Enemy( ref )
{
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.5f;
	mSpeed = 4.f;
	mBoundingBox.hHeight = 1.0f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.25f;
}

Moleratman::Moleratman()
{
	mSpeed = 2.f;
	mSound = nullptr;
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.5f;
	mBoundingBox.hHeight = 1.0f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.25f;
}

Moleratman::~Moleratman()
{
}