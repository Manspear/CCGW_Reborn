#include "Animation.h"
#include "Model.h"

void Animator::push( int animation, bool loop, float speed, float scale )
{
	sTake take = { animation, loop, speed, scale };
	mStack.push( take );
	mElapsed = 0.0f;
}

void Animator::pop()
{
	mStack.pop();
}

void Animator::clear()
{
	while( !mStack.empty() )
		mStack.pop();
}

void Animator::update( float dt )
{
	if( !mStack.empty() )
	{
		sTake& take = mStack.top();
		sAnimation* animation = pModel->getAnimation(take.mIndex);
		mElapsed += dt*take.mSpeed;

		if( mElapsed >= animation->mDuration*take.mScale )
		{
			if( take.mLooping )
				mElapsed -= animation->mDuration*take.mScale;
			else if( mStack.size() > 1 )
			{
				mElapsed = 0.0f;
				pop();
			}
			else // if this is the only animation and it's not looping, stop on the last keyframe
				mElapsed = animation->mDuration*take.mScale;
		}
	}
}

void Animator::setModel( Model* model )
{
	pModel = model;
}

sAnimation* Animator::getCurrentAnimation()
{
	if( mStack.size() <= 0 )
		return nullptr;
	return pModel->getAnimation(mStack.top().mIndex);
}

int Animator::getCurrentTake()
{
	if( mStack.size() <= 0 )
		return -1;
	return mStack.top().mIndex;
}

float Animator::getElapsed() const
{
	return mElapsed;
}

int Animator::getStackSize()
{
	return mStack.size();
}

Animator::Animator()
{
}

Animator::~Animator()
{
}