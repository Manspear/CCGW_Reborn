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

	newPos += movement * dt;

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
	if( mLookat.z < 0.0f )
		rotY *= -1.0f;

	// set data for rendering
	mPosition = newPos;
	mWorld = {
		cosf( rotY ),	0,		-sinf( rotY ),	0,
		0,				1,		0,				0,
		sinf(rotY),		0,		cosf(rotY),		0,
		mPosition.x,	mPosition.y+sin(mSin++*0.1f )*0.1f,	mPosition.z, 1
	};
	if (mLife <= 0)
		mAlive = false;

	mBoundingBox.center = mPosition - glm::vec3( 0.0f, 0.25f, 0.0f );
	mHeadBox.center = mPosition + glm::vec3( 0.0f, 0.25f, 0.0f );
}

void Molebat::render( GLuint programID )
{
	Enemy::render( programID );

	GLuint worldLocation = glGetUniformLocation(programID, "world");
	glm::mat4 world;
	world[3][0] = mBoundingBox.center.x;
	world[3][1] = mBoundingBox.center.y;
	world[3][2] = mBoundingBox.center.z;

	world[0][0] = mBoundingBox.hWidth*2.0f;
	world[1][1] = mBoundingBox.hHeight*2.0f;
	world[2][2] = mBoundingBox.hDepth*2.0f;

	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);

	//mpMesh->draw();
	mpModel->draw();

	world[3][0] = mHeadBox.center.x;
	world[3][1] = mHeadBox.center.y;
	world[3][2] = mHeadBox.center.z;

	world[0][0] = mHeadBox.hWidth*2.0f;
	world[1][1] = mHeadBox.hHeight*2.0f;
	world[2][2] = mHeadBox.hDepth*2.0f;

	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);

	//mpMesh->draw();
	mpModel->draw();
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
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.5f;
	mBoundingBox.hHeight = 0.25f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.25f;
}

Molebat::Molebat()
	: Enemy( glm::vec3( 0.0f ) ), mSin( rand() % 1000 )
{
	mTimeSinceLastHit = 0;
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.5f;
	mBoundingBox.hHeight = 0.25f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.25f;
}

Molebat::~Molebat()
{
}