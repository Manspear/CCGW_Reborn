#include "Moleratman.h"
#include "Player.h"

void Moleratman::update(float dt, GameData* data)
{
	if (mCurrent >= 0)
	{
		// TODO: Factor out box scale
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

		float p = glm::pi<float>()*0.5f;
		rotY -= p;

		mWorld = {
			cosf(rotY),	0,		-sinf(rotY),	0,
			0,				1,		0,				0,
			sinf(rotY),		0,		cosf(rotY),		0,
			mPosition.x,	mPosition.y,	mPosition.z, 1
		};
	mBoundingBox.center = mPosition + glm::vec3( 0, 0.75f, 0 );

	glm::vec3 headOffset = mLookat*0.5f;
	headOffset.y = 1.5f;
	mHeadBox.center = mPosition + headOffset;
}

//void Moleratman::render( GLuint programID )
void Moleratman::render( GLuint worldLocation, GLuint animationLocation )
{
	//Enemy::render( programID );
	Enemy::render( worldLocation, animationLocation );
	
#if ENEMY_RENDER_HITBOX
	glPolygonMode( GL_FRONT, GL_LINE );

	//GLuint worldLocation = glGetUniformLocation( programID, "world" );
	glm::mat4 world;
	world[3][0] = mBoundingBox.center.x;
	world[3][1] = mBoundingBox.center.y;
	world[3][2] = mBoundingBox.center.z;

	world[0][0] = mBoundingBox.hWidth*2.0f;
	world[1][1] = mBoundingBox.hHeight*2.0f;
	world[2][2] = mBoundingBox.hDepth*2.0f;

	glUniformMatrix4fv( worldLocation, 1, GL_FALSE, &world[0][0] );

	pBoundingBoxModel->drawNonAni();

	world[3][0] = mHeadBox.center.x;
	world[3][1] = mHeadBox.center.y;
	world[3][2] = mHeadBox.center.z;

	world[0][0] = mHeadBox.hWidth*2.0f;
	world[1][1] = mHeadBox.hHeight*2.0f;
	world[2][2] = mHeadBox.hDepth*2.0f;

	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &world[0][0]);

	pBoundingBoxModel->drawNonAni();

	glPolygonMode(GL_FRONT, GL_FILL );
#endif
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
	mBoundingBox.hWidth = mBoundingBox.hDepth = 0.25f;
	mBoundingBox.hHeight = 0.5f;
	mHeadBox.hWidth = mHeadBox.hHeight = mHeadBox.hDepth = 0.125f;
}

Moleratman::~Moleratman()
{
}