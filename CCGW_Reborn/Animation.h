#pragma once

#include <stack>
#include <map>
#include <string>
#include <vector>
#include <Mole\MoleReader.h>

struct sAnimation
{
	int mTake;
	float mDuration;
};

class Model;
class Animator
{
public:
	void push( int animation, bool loop, float speed = 1.0f, float scale = 1.0f, float offset = 0.0f );
	void pop();
	void clear();
	void update( float dt );
	void setElapsed(float percentage);
	bool isEmpty();
	void setModel( Model* model );
	sAnimation* getCurrentAnimation();
	int getCurrentTake();
	float getElapsed() const;
	int getStackSize();

	Animator();
	~Animator();

private:
	struct sTake
	{
		int mIndex;
		bool mLooping;
		float mSpeed, mScale;
	};

	Model* pModel;
	std::stack<sTake> mStack;
	float mElapsed;
};