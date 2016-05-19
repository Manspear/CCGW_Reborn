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

						mWeapon.shoot(mPosition , direction, angle);

						// set shooting animation
						if(mCrossbowAnimator.getCurrentTake() == ANIM_IDLE )
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
			//mpWeapon->update(dt);
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
			targetEnemy->imHit(mStrength, targetEnemy->getPosition() + glm::vec3(0,1.5f,0));
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

//void Tower::render(const GLuint & programID)
/*void Tower::render( GLuint worldLocation )
{
	// TODO: Please fix, don't ship
	float ypos = mWorld[3][1];
	mWorld[3][1] = 0.0f;

	GameObject::renderNonAni(worldLocation);
	if (mShooting)
		mWeapon.render( worldLocation );

	mWorld[3][1] = ypos;
}*/

void Tower::updateAnimation()
{
	Animator* animator = &mAnimator;
	if( mpModel == mpCrossbowModel )
		animator = &mCrossbowAnimator;

	mpModel->updateAnimation( 1.0f, animator->getCurrentTake(), animator->getElapsed() );
}

void Tower::renderNonAni( GLuint worldLocation )
{
	mWorld[3][1] = 0.0f;

	if( mHasBallista )
	{
		mpModel = mpBaseModel;
		GameObject::renderNonAni(worldLocation);
	}
	else
	{
		mpModel = mpBoxModel;
		GameObject::renderNonAni( worldLocation );
	}
}

void Tower::renderAni( GLuint worldLocation, GLuint animationLocation )
{
	//mpModel = mpBallistaModel;

	glm::mat4 prevWorld = mWorld;
	mWorld = mCrossbowMatrix;
	mWorld[3][1] = 0.0f;

	mpModel = mpCrossbowModel;
	GameObject::renderAni( worldLocation, animationLocation );

	mWorld = prevWorld;
	mWorld[3][0] += 1.0f;
	mWorld[3][2] -= 1.0f;
	mpModel = mpLidModel;
	GameObject::renderAni(worldLocation, animationLocation);

	mWorld = prevWorld;
	mWorld[3][1] = 0.0f;

	mpModel = mpSmallCylinderModel;
	GameObject::renderAni( worldLocation, animationLocation );

	mpModel = mpMidCylinderModel;
	GameObject::renderAni( worldLocation, animationLocation );

	mpModel = mpHighWheelModel;
	GameObject::renderAni( worldLocation, animationLocation );

	/*mpModel = mpHeadModel;
	GameObject::renderAni( worldLocation, animationLocation );*/
}

void Tower::renderArrows( GLuint worldLocation )
{
	mWeapon.render( worldLocation );
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
	mAnimator.push( 0, false, 1.0f, 0.95f );

	mCrossbowAnimator.setModel( mpCrossbowModel );
	mCrossbowAnimator.push( ANIM_IDLE, true );
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

/*Tower::Tower(GameData* gameData, glm::vec3 position, const Tower &towerRef, float scale): GameObject(position, scale){
	this->mpModel = towerRef.mpModel;
	this->mpWeapon = new Weapon(false, gameData);
	this->mWeaponReady = true;
	mLookat = { 1 ,0, 0 };
	mReloadTime = 5;
	mFireRate = 3;
	mShooting = false;
	mRange = 10;
	mStrength = 2;
}*/

Tower::Tower()
	: mWeaponReady( true ), mHasBallista( false ), mReloadTime( 5 ),
	mFireRate( 3 ), mShooting( false ), mRange( 10 ), mStrength( 1 ),
	targetEnemy( nullptr )
{
	/*mWeaponReady = true;
	mLookat = { 1 ,0, 0 };
	mReloadTime = 5;
	mFireRate = 3;
	mShooting = false;
	mRange = 10;
	mStrength = 2;*/
}

/*Tower::Tower()
{
	mpWeapon = nullptr;
}*/

Tower::~Tower()
{
	//delete mpWeapon;
}
