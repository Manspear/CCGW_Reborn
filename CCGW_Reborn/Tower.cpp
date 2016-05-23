#include "Tower.h"
#define GLM_FORCE_RADIANS
#include <glm\gtx\vector_angle.hpp>

bool Tower::load( GameData* data, glm::vec3 position, Model* boxModel, Model** models, Emitter* emitter )
{
	//mpWeapon = new Weapon( false, data );
	mWeapon.load( data, false, emitter );
	setPosition( position );
	setScale( data->boxScale / 2 );
	/*mpModel = mpBoxModel = boxModel;
	mpBallistaModel = ballistaModel;
	mpHeadModel = headModel;*/

	mpBoxModel = boxModel;

	for( int i=0; i<TOWER_MODELS; i++ )
		mpModels[i] = models[i];

	return true;
}

void Tower::update(GameData* gameData, const float & dt)
{
	//GameObject::update( dt );
	mAnimator.update(dt);
	mCrossbowAnimator.update(dt);

	if( mHasBallista )
	{
		if (mWeaponReady)
		{
			for (int i = 0; i < gameData->mMoleratmen; i++) {
				if (gameData->pMoleratmen[i].getAlive()) {
					glm::vec3 direction = (gameData->pMoleratmen[i].getPosition() + glm::vec3(0,1,0)) - mPosition;
					mDistanceToTarget = glm::length(direction);
					direction = glm::normalize(direction);
					if (mDistanceToTarget < mRange)
					{
						mWeaponReady = false;
						mShooting = true;
						mReloadTime = 3;
						targetEnemy = &gameData->pMoleratmen[i];
						glm::vec3 tempDir = glm::normalize(glm::vec3(direction.x, 0, direction.z));
						float angle = glm::angle(tempDir, glm::vec3(1, 0, 0));
						if (tempDir.z < 0)
							angle *= -1;

						mWeapon.shoot(mPosition , direction, angle, TOWERDAMAGE);

						// set shooting animation
						//if(mCrossbowAnimator.getCurrentTake() == ANIM_IDLE )
						if( mCrossbowAnimator.getStackSize() == 1 )
							mCrossbowAnimator.push( ANIM_SHOOTING, false );
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
			mWeapon.update( dt );
			mShooting = arrowShot(dt, gameData);
		}

		if( targetEnemy != nullptr )
		{
			if (mCrossbowAnimator.getCurrentTake() != ANIM_BUILDING)
			{
				glm::vec3 dir = glm::normalize(mPosition - targetEnemy->getPosition());
				float angle = glm::angle( dir, glm::vec3( 0, 0, 1 ) );
				if( dir.x < 0 ) angle *= -1.0f;

				mCrossbowMatrix = glm::mat4(
					cosf(angle), 0, -sinf(angle), 0,
					0, 1, 0, 0,
					sinf(angle), 0, cosf(angle), 0,
					mPosition.x, mPosition.y, mPosition.z, 1
				);
			}
		}
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
			targetEnemy->imHit(TOWERDAMAGE, targetEnemy->getPosition() + glm::vec3(0,1.5f,0));
			if (!targetEnemy->getAlive())
			{
				data->pGold++;
			}
		}
	}
	return targetHit;
}

void Tower::updateAnimation()
{
	for( int i=0; i<TOWER_MODELS; i++ )
	{
		if( mpModels[i] == mpCrossbowModel )
			mpModels[i]->updateAnimation( mJointMatrixList, 1.0f, mCrossbowAnimator.getCurrentTake(), mCrossbowAnimator.getElapsed() );
		else
			mpModels[i]->updateAnimation( mJointMatrixList, 1.0f, mAnimator.getCurrentTake(), mAnimator.getElapsed() );
	}
}

void Tower::renderNonAni( GLuint worldLocation, GLuint tintLocation )
{
	mWorld[3][1] = 0.0f;

	if( mHasBallista )
	{
		mpModel = mpBaseModel;
		GameObject::renderNonAni(worldLocation, tintLocation);
	}
	else
	{
		mpModel = mpBoxModel;
		GameObject::renderNonAni( worldLocation, tintLocation );
	}
}

void Tower::renderAni( GLuint worldLocation, GLuint animationLocation )
{
	//mpModel = mpBallistaModel;

	glm::mat4 prevWorld = mWorld;
	mWorld = mCrossbowMatrix;
	mWorld[3][1] = 0.0f;

	mpCrossbowModel->updateAnimation( mJointMatrixList, 1.0f, mCrossbowAnimator.getCurrentTake(), mCrossbowAnimator.getElapsed() );

	mpModel = mpCrossbowModel;
	GameObject::renderAni( worldLocation, animationLocation );

	mWorld = prevWorld;
	mWorld[3][0] += 1.0f;
	mWorld[3][2] -= 1.0f;
	mpModel = mpLidModel;
	mpModel->updateAnimation(mJointMatrixList, 1.0f, mAnimator.getCurrentTake(), mAnimator.getElapsed());
	GameObject::renderAni(worldLocation, animationLocation);

	mWorld = prevWorld;
	mWorld[3][1] = 0.0f;

	mpModel = mpSmallCylinderModel;
	mpModel->updateAnimation(mJointMatrixList, 1.0f, mAnimator.getCurrentTake(), mAnimator.getElapsed());
	GameObject::renderAni( worldLocation, animationLocation );

	mpModel = mpMidCylinderModel;
	mpModel->updateAnimation(mJointMatrixList, 1.0f, mAnimator.getCurrentTake(), mAnimator.getElapsed());
	GameObject::renderAni( worldLocation, animationLocation );

	mpModel = mpHighWheelModel;
	mpModel->updateAnimation(mJointMatrixList, 1.0f, mAnimator.getCurrentTake(), mAnimator.getElapsed());
	GameObject::renderAni( worldLocation, animationLocation );

	mWorld = prevWorld;
}

void Tower::renderArrows( GLuint worldLocation, GLuint tintLocation )
{
	mWeapon.render( worldLocation, tintLocation );
}

void Tower::setAlive( bool alive )
{
	mAlive = alive;
}

void Tower::setHasBallista( bool hasBallista )
{
	mHasBallista = hasBallista;
	//mpModel = ( hasBallista ? mpBallistaModel : mpBoxModel );

	mAnimator.setModel(mpLidModel);
	// only do 95% of the animation, or it will snap back to the first frame
	mAnimator.push( 0, false, 1.0f, 0.75f );

	mCrossbowAnimator.setModel( mpCrossbowModel );
	mCrossbowAnimator.push( ANIM_SHOOTING, false, 0.0f );
	mCrossbowAnimator.push( ANIM_BUILDING, false );
	mCrossbowMatrix = mWorld;
}

bool Tower::getAlive() const
{
	return mAlive;
}

bool Tower::getHasBallista() const
{
	return mHasBallista;
}

Tower::Tower()
	: mWeaponReady( true ), mHasBallista( false ), mReloadTime( 5 ),
	mFireRate( 1 ), mShooting( false ), mRange( 10 ), mStrength( 1 ),
	targetEnemy(nullptr)
{

}

Tower::~Tower()
{
	//delete mpWeapon;
}
