#include "Animation.h"
#include "Model.h"

void Animator::push( int animation, bool loop, float speed )
{
	sTake take = { animation, loop, speed };
	mStack.push(take);
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
		sAnimation* animation = pModel->getAnimation(mStack.top().mIndex);
		mElapsed += dt*mStack.top().mSpeed;
		if( mElapsed >= animation->mDuration )
		{
			if( mStack.top().mLooping )
				mElapsed -= animation->mDuration;
			else
				pop();
		}
	}
}

void Animator::setModel( Model* model )
{
	pModel = model;
}

sAnimation* Animator::getCurrentAnimation()
{
	return pModel->getAnimation(mStack.top().mIndex);
}

float Animator::getElapsed() const
{
	return mElapsed;
}

Animator::Animator()
{
}

Animator::~Animator()
{
}