#include "Enemy.h"

void Enemy::renderHitbox( GLuint worldLocation, GameObject* objects )
{
	glm::vec3 corners[8];
	mBoundingBox.getCorners( corners );

	for( int i=0; i<8; i++ )
	{
		objects[i].setPosition( corners[i] );
		objects[i].setScale( 0.1f );
		objects[i].renderNonAni( worldLocation );
	}

	mHeadBox.getCorners( corners );

	for( int i=0; i<8; i++ )
	{
		objects[i].setPosition( corners[i] );
		objects[i].setScale( 0.1f );
		objects[i].renderNonAni( worldLocation );
	}
}

void Enemy::setPath( sNode* path, int max )
{
	pPath = path;
	mCurrent = max-1;
}

void Enemy::load(Model* model, Emitter* emitter)
{
	GameObject::load(model);
	this->mEmitter = *emitter;
}

void Enemy::imHit(float strength, glm::vec3 position)
{
	mLife -= strength;
	if (mSound != nullptr)
		mSound->play();
	if (mLife <= 0) {
		mAlive = false;
	}
	mEmitter.spawn(position, glm::vec3(0, -1.0f, 0), 1.0f);
	mEmitter.spawn(position, glm::vec3(1.0f, -0.5f, 0), 1.0f);
	mEmitter.spawn(position, glm::vec3(0, -0.5f, 1.0f), 1.0f);
	mEmitter.spawn(position, glm::vec3(0, -0.5f, -1.0f), 1.0f);
	mEmitter.spawn(position, glm::vec3(-1.0f, -0.5f, 0.0f), 1.0f);
}

void Enemy::setAlive( bool alive )
{
	mAlive = alive;
}

void Enemy::setLife( float life )
{
	mLife = life;
}

void Enemy::setSpeed(float newSpeed)
{
	mSpeed = newSpeed;
}

bool Enemy::getAlive() const
{
	return mAlive;
}

float Enemy::getLife() const
{
	return mLife;
}

float Enemy::getSpeed() const
{
	return mSpeed;
}

const BoundingBox& Enemy::getBoundingBox() const
{
	return mBoundingBox;
}

const BoundingBox& Enemy::getHeadBox() const
{
	return mHeadBox;
}

Enemy& Enemy::operator=( const Enemy& ref )
{
	GameObject::operator=( ref );
	mCurrent = ref.mCurrent;
	pPath = ref.pPath;
	mAlive = ref.mAlive;
	return *this;
}

Enemy::Enemy( const Enemy& ref )
	: GameObject( ref ), mCurrent( ref.mCurrent ), pPath( ref.pPath ), mAlive( ref.mAlive ), mBoundingBox( ref.mBoundingBox ), mPrevTargetDif( ref.mPrevTargetDif )
{
}

Enemy::Enemy( glm::vec3 position )
	: GameObject( position, 1.0f ), mCurrent( -1 ), pPath( nullptr ), mAlive( false ), mLife(1), mBoundingBox( position, 0.5f )
{
	this->mBoundRadius = 1.0f;
	mPrevTargetDif = { 0, 1 };
}

Enemy::Enemy()
	: mCurrent(-1), pPath(nullptr), mAlive(false), mLife(1), mBoundingBox( glm::vec3( 0.0f ), 0.5f )
{
	this->mBoundRadius = 1.0f;
	mPrevTargetDif = { 0, 1 };
}

Enemy::~Enemy()
{
}