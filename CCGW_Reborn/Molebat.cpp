#include "Molebat.h"

void Molebat::update(float dt)
{
	Enemy::update( dt );

	mTimeSinceLastHit += dt;
	glm::vec3 newPos = mPosition;
	glm::vec3 movement;

	// chase the player
	glm::vec3 dir = pGameData->pPlayer->getPosition() - mPosition;
	float dist = glm::length( dir );
	if( dist > MOLEBAT_FAR_DISTANCE )
	{
		dir = glm::normalize( dir );
		movement += dir;
	}
	else if( dist < MOLEBAT_NEAR_DISTANCE )
	{
		dir = -glm::normalize( dir );
		movement += dir;
	}
	else if(mTimeSinceLastHit > 1){

		// play the molebats attack animation
		mAnimator.push( 3, false, 2.0f );

		pGameData->pPlayer->takeDamage(1);
		mTimeSinceLastHit = 0;
	}

	newPos += movement * mSpeed * dt;

	// stay above ground
	if( newPos.y < MOLEBAT_HEIGHT )
		newPos.y = MOLEBAT_HEIGHT;

	// evade other molebats
	glm::vec3 evade;
	for( int i=0; i<pGameData->mMolebats; i++ )
	{
		if( &pGameData->pMolebats[i] == this || !pGameData->pMolebats[i].getAlive() )
			continue;

		glm::vec3 dir = pGameData->pMolebats[i].getPosition() - newPos;
		float dist = glm::length( dir );
		if( dist < MOLEBAT_ALLY_DISTANCE )
		{
			dir = -glm::normalize( dir );
			dist = MOLEBAT_ALLY_DISTANCE - dist;
			evade += dir * dist;
		}
	}

	newPos += evade;

	// face the player
	dir = glm::normalize( pGameData->pPlayer->getPosition() - mPosition );
	mLookat = glm::normalize( glm::vec3( dir.x, 0.0f, dir.z ) );
	float rotY = -glm::angle( mLookat, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	float p = glm::pi<float>()*0.5f;
	float scale = 0.2f;
	
	if( mLookat.z < 0.0f )
		rotY *= -1.0f;

	rotY -= p;
	float sinOffset = sin( mSin++*0.1f ) * 0.1f;

	// set data for rendering
	mPosition = newPos;
	mWorld = {
		cosf( rotY ) * scale,	0,		-sinf( rotY ) * scale,	0,
		0,				1 * scale,		0,				0,
		sinf(rotY) * scale,		0,		cosf(rotY) * scale,		0,
		mPosition.x,	mPosition.y+sinOffset,	mPosition.z, 1
	};
	if (mLife <= 0)
		mAlive = false;

	glm::vec3 bodyOffset = -mLookat*0.5f;
	bodyOffset.y = -0.5f + sinOffset;

	glm::vec3 headOffset = mLookat*0.25f;
	headOffset.y = -0.3f + sinOffset;

	mBoundingBox.center = mPosition + bodyOffset;
	mHeadBox.center = mPosition + headOffset;
}

void Molebat::imHit( float strength, glm::vec3 position )
{
	Enemy::imHit( strength, position );

	mAnimator.push( 2, false );
}

void Molebat::setGameData( GameData* data )
{
	pGameData = data;
}

Molebat& Molebat::operator=( const Molebat& ref )
{
	Enemy::operator=( ref );
	mSin = ref.mSin;
	return *this;
}

Molebat::Molebat( const Molebat& ref )
	: Enemy( ref ), pGameData( ref.pGameData ), mSin( ref.mSin )
{
	mSpeed = 3.f;
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Molebat::Molebat()
	: Enemy( glm::vec3( 0.0f ) ), mSin( rand() % 1000 )
{
	mSpeed = 3.f;
	mTimeSinceLastHit = 0;
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Molebat::~Molebat()
{
}