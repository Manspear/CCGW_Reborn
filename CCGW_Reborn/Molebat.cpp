#include "Molebat.h"

void Molebat::update(float dt)
{
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
		pGameData->pPlayer->takeDamage(10);
		mTimeSinceLastHit = 0;
	}
	float mSpeed = 3;
	newPos += movement * mSpeed * dt;

	// stay above ground
	if( newPos.y < MOLEBAT_HEIGHT )
		newPos.y = MOLEBAT_HEIGHT;

	// evade other molebats
	glm::vec3 evade;
	for( int i=0; i<pGameData->mMolebats; i++ )
	{
		if( &pGameData->pMolebats[i] == this )
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
	headOffset.y = sinOffset;

	mBoundingBox.center = mPosition + bodyOffset;
	mHeadBox.center = mPosition + headOffset;
}

void Molebat::render( GLuint programID )
{
	Enemy::render( programID );
	
#if ENEMY_RENDER_HITBOX
	glPolygonMode( GL_FRONT, GL_LINE );

	GLuint worldLocation = glGetUniformLocation(programID, "world");
	glm::mat4 world;
	world[3][0] = mBoundingBox.center.x;
	world[3][1] = mBoundingBox.center.y;
	world[3][2] = mBoundingBox.center.z;

	world[0][0] = mBoundingBox.hWidth*2.0f;
	world[1][1] = mBoundingBox.hHeight*2.0f;
	world[2][2] = mBoundingBox.hDepth*2.0f;

	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);

	pBoundingBoxModel->drawNonAni();

	world[3][0] = mHeadBox.center.x;
	world[3][1] = mHeadBox.center.y;
	world[3][2] = mHeadBox.center.z;

	world[0][0] = mHeadBox.hWidth*2.0f;
	world[1][1] = mHeadBox.hHeight*2.0f;
	world[2][2] = mHeadBox.hDepth*2.0f;

	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);

	pBoundingBoxModel->drawNonAni();

	glPolygonMode( GL_FRONT, GL_FILL );
#endif
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
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Molebat::Molebat()
	: Enemy( glm::vec3( 0.0f ) ), mSin( rand() % 1000 )
{
	mTimeSinceLastHit = 0;
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Molebat::~Molebat()
{
}