#include "Particle.h"

bool Particle::load( GameData* data, string texture )
{
	mpTexture = data->pAssets->load<Texture>( texture );
	return ( mpTexture != nullptr );
}

void Particle::spawn( glm::vec3 position, glm::vec3 velocity, float lifetime, float drag, glm::vec2 startScale, glm::vec2 endScale )
{
	mPosition = position;
	mVelocity = velocity;
	mLifetime = lifetime;
	mStartScale = mScale = startScale;
	mEndScale = endScale;
	mDrag = drag;
	mElapsed = 0.0f;
	mShouldAdd = true;
}

void Particle::update( const glm::vec3& cameraPosition, float deltaTime )
{
	if( mElapsed < mLifetime )
	{
		mPosition += mVelocity *deltaTime;
		mVelocity *= 1.0f - mDrag *deltaTime;
	
		float a = mElapsed / mLifetime;
		mScale = ((1 - a) * mStartScale) + ( mEndScale * a );

		mElapsed += deltaTime;
	}
}

void Particle::draw( Camera* camera, BillboardProgram* billboardProgram )
{
	if( mElapsed < mLifetime )
	{
		mpTexture->bind();
		billboardProgram->draw( camera, mPosition, mScale );
	}
}

glm::vec3 Particle::getPosition() const
{
	return mPosition;
}

glm::vec3 Particle::getVelocity() const
{
	return mVelocity;
}

glm::vec2 Particle::getScale() const
{
	return mScale;
}

float Particle::getLifetime() const
{
	return mLifetime;
}

float Particle::getElapsed() const
{
	return mElapsed;
}

bool Particle::getShouldAdd()
{
	bool result = mShouldAdd;
	mShouldAdd = false;
	return result;
}

Particle& Particle::operator=( const Particle& ref )
{
	mPosition = ref.mPosition;
	mVelocity = ref.mVelocity;
	mScale = ref.mScale;

	mStartScale = ref.mStartScale;
	mEndScale = ref.mEndScale;

	mLifetime = ref.mLifetime;
	mElapsed = ref.mElapsed;
	mDrag = ref.mDrag;
	mShouldAdd = ref.mShouldAdd;

	mpTexture = ref.mpTexture;
	return *this;
}

Particle::Particle( const Particle& ref )
	: mPosition( ref.mPosition ), mVelocity( ref.mVelocity ), mScale( ref.mScale ),
	mStartScale( ref.mStartScale ), mEndScale( ref.mEndScale ),
	mLifetime( ref.mLifetime ), mElapsed( ref.mElapsed ), mDrag( ref.mDrag ), mShouldAdd( ref.mShouldAdd ),
	mpTexture( ref.mpTexture )
{
}

Particle::Particle()
	: mPosition( 0.0f ), mVelocity( 0.0f ), mScale( 1.0f ),
	mStartScale( 1.0f ), mEndScale( 1.0f ),
	mLifetime( 0.0f ), mElapsed( 1.0f ), mDrag( 0.0f ), mShouldAdd( false ),
	mpTexture( nullptr )
{
}

Particle::~Particle()
{
}