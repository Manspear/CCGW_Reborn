#include "Player.h"
#include <iostream>
#define GLM_FORCE_RADIANS
#include "global_variables.h"
#include "Grid.h"
#include "glm\gtx\vector_angle.hpp"

glm::vec3 Player::getLookAt() const {
	return this->mLookat;
}

float Player::getRot() const
{
	return rotX;
}
float Player::getStrength() const
{
	return mStrength;
}
float Player::getYOffset() {
	return this->yoffset;
}

void Player::update(const Input* inputs, const float &dt)
{
	GameObject::update( dt );

	mWorld = glm::mat4();
	bool canJump = false;

	//mWeapon->update(dt);
	mWeapon.update(dt);
	speedY -= 25 * dt;
	mSpeed *= 1 - 15 * dt;
	if (mSpeed < 0)
		mSpeed = 0;
	//mSpeed = 0;
	glm::vec3 tempLookat = glm::normalize(glm::vec3(mLookat.x, 0, mLookat.z));
	glm::vec3 dir(0.0f, 0.0f, 0.0f);

	attackReadyTimer += dt;
	animationTimer += dt;
	//buttonDown(0) == LMB
	/**
	How to loop ONE animation?
	Need to be able to loop in a state. When an action is done, exit that state temporarily.
	Return to it as a sort of "default animation" when the action is done. Use pop() to remove the topmost element.

	if(nobuttondown)
		startState1AnimTimer
		while state1AnimTimer < animMax:
			play state1[x]
	**/

	if (inputs->buttonDown(0))
	{
		mStrength > 5 ? mStrength = 5 : mStrength += dt * 3 ;
	}

	if (inputs->buttonReleased(0))
	{
		attackReadyTimer = 0;
		if (mStrength > 0.5f)
		{
			attackReadyTimer = 0;
			glm::vec3 temp = { mLookat.x, 0, mLookat.z };
			glm::vec3 tempPos = this->mPosition + glm::cross(glm::normalize(temp), glm::vec3(0, 1, 0)) / 4.f - glm::vec3(0, yoffset*2, 0);
			glm::vec3 la = glm::normalize((mPosition + 10.0f*mLookat) - tempPos);
			//glm::vec3 la = glm::normalize((mPosition + mLookat) - tempPos);
			float rotation = rotX - glm::angle(glm::normalize(glm::vec3(la.x, 0, la.z)), tempLookat);
			
		    float damage = (mStrength / 5) * 10.f;

			mWeapon.shoot(tempPos, la, rotation, mStrength, damage);

			/*mAnimator.push( 3, false, 5.0f );

			mAnimator.clear();
			mAnimator.push(ANIM_RUN, true, 1.0f);
			mAnimator.push(ANIM_RELOAD, false, 1.0f);*/
		}
		mStrength = 0;
	}


	if (inputs->keyDown(SDLK_a))
	{
		mSpeed = mMaxSpeed / 2;
		float r = glm::pi<float>() * 0.5f;
		dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	}
	if (inputs->keyDown(SDLK_d))
	{
		mSpeed = mMaxSpeed / 2;
		float r = glm::pi<float>() * 1.5f;
		dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	}
	if (inputs->keyDown(SDLK_w))
	{
		mSpeed = mMaxSpeed;
		//dir += glm::vec3(cos(rotX), 0.0f, sin(rotX));
		dir += glm::vec3(cos(rotX), 0.0f, sin(rotX));
	}
	if(inputs->keyDown(SDLK_s))
	{
		mSpeed = mMaxSpeed / 2;
		float r = glm::pi<float>();
		dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	}

	std::cout << state << endl;

	if (state == 1 && (inputs->buttonDown(0) || inputs->buttonPressed(0)))
	{
		if (!enteredStates)
		{
			enteredStates = true;
			animationTimer = 0;
		}
		if (inputs->keyReleased(SDLK_w))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_s))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_a))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_d))
			stateRunning = false;
		//Start the run-to-attack animation
		if (mSpeed < 0.05 && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(17, false, mAnimationSpeed, 1.0f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_w) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(2, false, mAnimationSpeed, 1.0f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_s) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(9, false, mAnimationSpeed, 1.0f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_a) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(22, false, mAnimationSpeed, 1.0f, animationTimer);
		}
		else if ( inputs->keyDown(SDLK_d) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(28, false, mAnimationSpeed, 1.0f, animationTimer);
		}
		//When the animation is done
		sAnimation* currAnim = mAnimator.getCurrentAnimation();
		if (animationTimer >= currAnim->mDuration - 0.5)
		{
			state = 2;
			animationTimer = 0;
			stateRunning = false;
		}
	}
	else if (state == 2)
	{
		if (inputs->keyReleased(SDLK_w))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_s))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_a))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_d))
			stateRunning = false;

		//Start the start of the attack animation
		if (mSpeed < 0.05 && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(18, false, mAnimationSpeed, 0.5f, animationTimer);
		}
		else if ( inputs->keyDown(SDLK_w) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(3, false, mAnimationSpeed, 0.5f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_s) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(10, false, mAnimationSpeed, 0.5f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_a) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(23, false, mAnimationSpeed, 0.5f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_d) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(29, false, mAnimationSpeed, 0.5f, animationTimer);
		}
		sAnimation* currAnim = mAnimator.getCurrentAnimation();
		if (animationTimer >= currAnim->mDuration * 0.5 - 0.25)
		{
			state = 3;
			animationTimer = 0;
			stateRunning = false;
		}
	}
	else if (state == 3)
	{
		//use the attack_17 animation
		if (inputs->keyReleased(SDLK_w))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_s))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_a))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_d))
			stateRunning = false;

		if (mSpeed < 0.05 && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(19, true, mAnimationSpeed, 1.0f, animationTimer);
			//mAnimator.push(18, false, mAnimationSpeed, 0.5f);
		}
		else if (inputs->keyDown(SDLK_w) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(4, true, mAnimationSpeed, 1.f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_s) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(11, true, mAnimationSpeed, 1.f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_a) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(24, true, mAnimationSpeed, 1.f, animationTimer);
		}
		else if ( inputs->keyDown(SDLK_d) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(30, true, mAnimationSpeed, 1.f, animationTimer);
		}
		if (inputs->buttonReleased(0) || inputs->buttonUp(0))
		{
			state = 4;
			animationTimer = 0;
			stateRunning = false;
		}
	}
	else if (state == 4)
	{
		if (inputs->keyReleased(SDLK_w))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_s))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_a))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_d))
			stateRunning = false;

		if (mSpeed < 0.05f && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(18, false, mAnimationSpeed, 1, 0.5 + animationTimer);
		}
		else if (inputs->keyDown(SDLK_w) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(3, false, mAnimationSpeed, 1.f, 0.5f + animationTimer);
		}
		else if (inputs->keyDown(SDLK_s) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(10, false, mAnimationSpeed, 1.f, 0.5f + animationTimer);
		}
		else if (inputs->keyDown(SDLK_a) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(23, false, mAnimationSpeed, 1.f, 0.5f + animationTimer);
		}
		else if (inputs->keyDown(SDLK_d) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(29, false, mAnimationSpeed, 1.f, 0.5f + animationTimer);
		}
		sAnimation* currAnim = mAnimator.getCurrentAnimation();
		if (animationTimer >= (currAnim->mDuration * 0.5 - 0.25))
		{
			state = 5;
			animationTimer = 0;
			stateRunning = false;
		}
	}
	else if (state == 5) 
	{
		if (inputs->keyReleased(SDLK_w))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_s))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_a))
			stateRunning = false;
		if (inputs->keyReleased(SDLK_d))
			stateRunning = false;

		if (mSpeed < 0.05f && !stateRunning)
		{
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(21, true, mAnimationSpeed, 1, animationTimer);
		}
		else if ( inputs->keyDown(SDLK_w) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(6, true, mAnimationSpeed, 1.f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_s) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(13, true, mAnimationSpeed, 1.f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_a) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(26, true, mAnimationSpeed, 1.f, animationTimer);
		}
		else if (inputs->keyDown(SDLK_d) && !stateRunning)
		{
			//animationTimer = 0;
			stateRunning = true;
			mAnimator.clear();
			mAnimator.push(32, true, mAnimationSpeed, 1.f, animationTimer);
		}
		if (inputs->buttonDown(0) || inputs->buttonPressed(0))
		{
			state = 2;
			animationTimer = 0;
			stateRunning = false;
		}
		else {
			mAnimator.clear();
			enteredStates = false;
			state = 1;
			animationTimer = 0;
			stateRunning = false;
		}
		/*sAnimation* currAnim = mAnimator.getCurrentAnimation();
		if (animationTimer >= currAnim->mDuration)
		{
			mAnimator.clear();
			state = 1;
		}*/
	}
	else
	{
		state = 1;
		//do usual run animations etc
		if (mSpeed < 0.01f)
		{
			cout << "IDLE" << endl;
			if (!isIdle)
			{
				isIdle = true;
				mAnimator.clear();
				mAnimator.push(15, true, 1, 1);
			}
		}
		else
			isIdle = false;

		if (inputs->keyDown(SDLK_a))
		{
			cout << "LEFT" << endl;
			if (!isStrafingLeft)
			{
				isRunning = false;
				isBacking = false;
				isStrafingLeft = true;
				mAnimator.clear();
				mAnimator.push(26, true, 1, 1);
			}
		}
		else
			isStrafingLeft = false;
		if (inputs->keyDown(SDLK_d))
		{
			cout << "RIGHT" << endl;
			if (!isStrafingRight)
			{
				isRunning = false;
				isBacking = false;
				isStrafingRight = true;
				mAnimator.clear();
				mAnimator.push(32, true, 1, 1);
			}
		}
		else
			isStrafingRight = false;

		if (inputs->keyDown(SDLK_s))
		{

			cout << "BACKING" << endl;
			if (!isBacking)
			{
				isBacking = true;
				mAnimator.clear();
				mAnimator.push(8, true, 1, 1);
			}
		}
		else
			isBacking = false;

		if (inputs->keyDown(SDLK_w))
		{
			cout << "RUNNING" << endl;
			if (!isRunning)
			{
				isRunning = true;
				mAnimator.clear();
				mAnimator.push(1, true, 1, 1);
			}
		}
		else
		{
			isRunning = false;
			if (inputs->keyReleased(SDLK_w))
			{
				isStrafingLeft = false;
				isStrafingRight = false;
			}
	
		}
			
	}
	if (mAnimator.isEmpty())
		mAnimator.push(15, false, 1, 1);











	//if (mSpeed < 0.05)
	//{
	//	if (!inputs->buttonDown(0) && idleAttackTimer > 2.f) //&& !isIdleAttacking)
	//	{
	//		if (!isIdle && attackReadyTimer > ATKRDYTIMER)
	//		{
	//			isIdle = true;
	//			mAnimator.clear();
	//			mAnimator.push(15, true, mAnimationSpeed, 1);
	//		}
	//		if (!isHoldIdle && attackReadyTimer < ATKRDYTIMER)
	//		{
	//			isHoldIdle = true;
	//			mAnimator.clear();
	//			mAnimator.push(21, true, mAnimationSpeed, 1);
	//		}
	//	}
	//	else
	//	{
	//		isIdle = false;
	//		isHoldIdle = false;
	//		if (!isIdleAttacking)
	//		{
	//			isIdleAttacking = true;
	//			mAnimator.clear();
	//			mAnimator.push(19, true, mAnimationSpeed, 1.0f);
	//			mAnimator.push(18, false, mAnimationSpeed, 0.5f);
	//			mAnimator.push(17, false, mAnimationSpeed, 1.0f);
	//		}
	//	}
	//	if (inputs->buttonReleased(0))
	//	{
	//		isIdleAttacking = false;
	//		mAnimator.push(21, true, mAnimationSpeed, 1.0f);
	//		mAnimator.push(18, false, mAnimationSpeed, 1, 0.5f);
	//	}
	//}
	//else 
	//{
	//	isIdle = false;
	//	isHoldIdle = false;
	//	isIdleAttacking = false;
	//}
	//if (startIdleAttackTimer)
	//{
	//	idleAttackTimer += dt;
	//	if (idleAttackTimer > 2.f)
	//	{
	//		startIdleAttackTimer = false;
	//	}
	//}

	//if (inputs->buttonDown(0))
	//{
	//	mStrength > 5 ? mStrength = 5 : mStrength += dt * 3 ;

	//	if( mStrength > 0.25f && mAnimator.getCurrentTake() != ANIM_SHOOT )
	//	{
	//		//mAnimator.clear();
	//		//if (true)//mSpeed > 0.5f) 
	//		//{
	//		//mAnimator.push( ANIM_SHOOT, false, 2.0f, 0.5f );
	//		//}
	//		//else {
	//		//	mAnimator.push(11, false, 2.0f, 0.5f);
	//		//}
	//	}
	//}

	//if (inputs->buttonReleased(0))
	//{
	//	attackReadyTimer = 0;
	//	if (mStrength > 0.5f)
	//	{
	//		attackReadyTimer = 0;
	//		glm::vec3 temp = { mLookat.x, 0, mLookat.z };
	//		glm::vec3 tempPos = this->mPosition + glm::cross(glm::normalize(temp), glm::vec3(0, 1, 0)) / 4.f - glm::vec3(0, yoffset*2, 0);
	//		glm::vec3 la = glm::normalize((mPosition + 10.0f*mLookat) - tempPos);
	//		//glm::vec3 la = glm::normalize((mPosition + mLookat) - tempPos);
	//		float rotation = rotX - glm::angle(glm::normalize(glm::vec3(la.x, 0, la.z)), tempLookat);
	//		
	//	    float damage = (mStrength / 5) * 10.f;

	//		mWeapon.shoot(tempPos, la, rotation, mStrength, damage);

	//		/*mAnimator.push( 3, false, 5.0f );

	//		mAnimator.clear();
	//		mAnimator.push(ANIM_RUN, true, 1.0f);
	//		mAnimator.push(ANIM_RELOAD, false, 1.0f);*/
	//	}
	//	mStrength = 0;
	//}


	//if (inputs->keyDown(SDLK_a))
	//{
	//	wasADown = true;
	//	if (inputs->buttonReleased(0))
	//	{
	//		mAnimator.clear();
	//		mAnimator.push(26, true, mAnimationSpeed, 1.0f);
	//		mAnimator.push(23, false, mAnimationSpeed, 1, 0.5f);

	//	}
	//	if (inputs->buttonDown(0))
	//	{
	//		if (!isLStrafeAttacking)
	//		{
	//			isLStrafeAttacking = true;
	//			//mAnimator.clear();
	//			mAnimator.push(24, true, mAnimationSpeed, 1.0f);
	//			mAnimator.push(23, false, mAnimationSpeed, 0.5f);
	//			mAnimator.push(22, false, mAnimationSpeed, 1.0f);
	//		}

	//	}
	//	else
	//	{
	//		isLStrafeAttacking = false;
	//		if (!isStrafingLeft)
	//		{

	//			mAnimator.clear();
	//			mAnimator.push(26, true, mAnimationSpeed, 1.0);
	//			isStrafingLeft = true;
	//		}
	//	}
	//	/*	mAnimator.clear();*/
	//	mSpeed = mMaxSpeed / 2;
	//	float r = glm::pi<float>() * 0.5f;
	//	dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	//}
	//else {
	//	isStrafingLeft = false;
	//	if (wasSDown)
	//	{
	//		wasSDown = false;
	//		mSpeed = 0;
	//		isRunning = false;
	//		isRunningHold = false;
	//	}
	//}

	//if (inputs->keyDown(SDLK_d))
	//{
	//	wasDDown = true;
	//	if (inputs->buttonReleased(0))
	//	{
	//			mAnimator.clear();
	//			mAnimator.push(32, true, mAnimationSpeed, 1.0f);
	//			mAnimator.push(29, false, mAnimationSpeed, 1, 0.5f);
	//	}
	//	if (inputs->buttonDown(0))
	//	{
	//		if (!isRStrafeAttacking)
	//		{
	//			isRStrafeAttacking = true;
	//			//mAnimator.clear();
	//			mAnimator.push(30, true, mAnimationSpeed, 1.0f);
	//			mAnimator.push(29, false, mAnimationSpeed, 0.5f);
	//			mAnimator.push(28, false, mAnimationSpeed, 1.0f);
	//		}
	//	}
	//	else
	//	{
	//		isRStrafeAttacking = false;
	//		if (!isStrafingRight)
	//		{
	//			mAnimator.clear();
	//			mAnimator.push(32, true, mAnimationSpeed, 1.0);
	//			isStrafingRight = true;
	//		}
	//	}
	//	/*	mAnimator.clear();*/
	//	mSpeed = mMaxSpeed / 2;
	//	float r = glm::pi<float>() * 1.5f;
	//	dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	//}
	//else
	//{
	//	isStrafingRight = false;
	//	if (wasDDown) 
	//	{
	//		wasDDown = false;
	//		mSpeed = 0;
	//		isRunning = false;
	//		isRunningHold = false;
	//	}
	//}

	//if (inputs->keyDown(SDLK_w))
	//{
	//	wasWDown = true;
	//	if (inputs->buttonReleased(0))
	//	{
	//		mAnimator.clear();
	//		mAnimator.push(6, true, mAnimationSpeed, 1.0f);
	//		mAnimator.push(3, false, mAnimationSpeed, 1, 0.5f);

	//	}
	//	else if (inputs->buttonDown(0))
	//	{
	//		if (!isRunningAttacking)
	//		{
	//			isRunningAttacking = true;
	//			mAnimator.clear();
	//			mAnimator.push(4, true, mAnimationSpeed, 1.0f);
	//			mAnimator.push(3, false, mAnimationSpeed, 0.5f);
	//			mAnimator.push(2, false, mAnimationSpeed, 1.0f);
	//		}
	//	}
	//	else {
	//		isRunningAttacking = false;
	//	}

	//	if (!inputs->buttonDown(0))
	//	{
	//		if (attackReadyTimer > ATKRDYTIMER && !isRunning)
	//		{
	//			//mAnimator.clear();
	//			mAnimator.push(1, true, mAnimationSpeed, 1);
	//			isRunning = true;
	//		}
	//		else if (attackReadyTimer < ATKRDYTIMER && !isRunningHold)
	//		{
	//			mAnimator.push(6, true, mAnimationSpeed, 1);
	//			isRunningHold = true;
	//		}
	//	}

	//	mSpeed = mMaxSpeed;
	//	//dir += glm::vec3(cos(rotX), 0.0f, sin(rotX));
	//	dir += glm::vec3(cos(rotX), 0.0f, sin(rotX));
	//}
	//else
	//{
	//	if (wasWDown)
	//	{
	//		wasWDown = false;
	//		mSpeed = 0;
	//	}
	//	isRunning = false;
	//	isRunningHold = false;
	//}

	//if (inputs->keyDown(SDLK_s))
	//{
	//	wasSDown = true;

	//	if (inputs->buttonReleased(0))
	//	{
	//		//isBackAttacking = false;
	//		mAnimator.clear();
	//		mAnimator.push(13, true, mAnimationSpeed, 1.0f);
	//		mAnimator.push(10, false, mAnimationSpeed, 1, 0.5f);

	//	}
	//	if(inputs->buttonDown(0))
	//	{ 
	//		if (!isBackAttacking)
	//		{
	//			isBackAttacking = true;
	//			mAnimator.clear();
	//			mAnimator.push(11, true, mAnimationSpeed, 1, 0.f);
	//			mAnimator.push(10, false, mAnimationSpeed, 0.5);
	//			//mAnimator.push(9, false, mAnimationSpeed, 1, 0);
	//		}
	//	}
	//	else
	//	{
	//		isBackAttacking = false;
	//		if (!isBacking && attackReadyTimer > ATKRDYTIMER)
	//		{
	//			mAnimator.clear();
	//			mAnimator.push(8, true, mAnimationSpeed, 1.f);
	//			isBacking = true;
	//		}
	//		if (!isBackingHold && attackReadyTimer < ATKRDYTIMER)
	//		{
	//			mAnimator.clear();
	//			mAnimator.push(13, true, mAnimationSpeed, 1.f);
	//			isBackingHold = true;
	//		}
	//	}
	//	
	//	mSpeed = mMaxSpeed / 2;
	//	float r = glm::pi<float>();
	//	dir += glm::vec3(cos(rotX - r), 0.0f, sin(rotX - r));
	//}
	//else
	//{
	//	if (wasSDown)
	//	{
	//		wasSDown = false;
	//		mSpeed = 0;
	//	}
	//	isBacking = false;
	//	isBackingHold = false;
	//	isBackAttacking = false;
	//}

	if(glm::length(dir) > 0.1f)
	{
		dir = glm::normalize(dir);
		mDirection = dir;
	}

	float dx = mDirection.x*mSpeed*dt;
	float dz = mDirection.z*mSpeed*dt;
	float dy = speedY*dt;
	
	glm::vec3 deltaPos = {0,0,0};
	if (checkMove({mPosition.x + dx,mPosition.y,mPosition.z }))
		deltaPos.x = dx;
	if (checkMove({ mPosition.x,mPosition.y + dy,mPosition.z }))
		deltaPos.y = dy;
	else {
		speedY = 0;
		canJump = true;
	}
	if (checkMove({ mPosition.x,mPosition.y,mPosition.z + dz }))
		deltaPos.z = dz;
	mPosition += deltaPos;

	if (inputs->keyPressed(SDLK_q)) {
		for (int i = 0; i < pGameData->pGrid->getHeight();i++) {
			for (int j = 0; j < pGameData->pGrid->getWidth(); j++) {
				std::cout << (int)pGameData->pGrid->getTile(j, i) << " ";
			}
			std::cout << std::endl;
		}
		int a = 0;
		std::cout << std::endl;
	}

	if (mPosition.y - yoffset < 0.5) {
		mPosition.y = 0.0;
		speedY = 0;
		canJump = true;
	} 

	if (inputs->keyPressed(SDLK_SPACE) && canJump)
	{
		//mAnimator.clear();
		mAnimator.push(34, false, mAnimationSpeed, 1);
		isJumping = true;	
	}
	if (isJumping) 
	{
		jumpTimer += dt;
		if (jumpTimer > 0.25f)
		{
			speedY += 12;
			jumpTimer = 0.f;
			isJumping = false;
		}
	}

	double degree = (inputs->mouseDelta().x) / 200 * -1;
	double rad = (inputs->mouseDelta().y) / 400 * -1;

	rotX += degree;

	if (rotX > glm::pi<float>()*2.0f)
		rotX -= glm::pi<float>()*2.0f;
	else if (rotX < 0.0f)
		rotX += glm::pi<float>()*2.0;

	glm::mat4 rotatematrix = { cosf(degree), 0, sinf(degree),0,
								0,1,0,0,
								-sinf(degree), 0, cosf(degree),0,
								0,0,0,1
	};

	float rotatePlayer = -PI / 2;

	glm::mat4 rmat = { cosf(rotX + rotatePlayer), 0, sinf(rotX + rotatePlayer),0,
		0,1,0,0,
		-sinf(rotX + rotatePlayer), 0, cosf(rotX + rotatePlayer),0,
		0,0,0,1
	};


	// #fuckedup maths, https://en.wikipedia.org/wiki/Rotation_matrix for formula
	glm::vec3 axis = glm::normalize(glm::cross(tempLookat, glm::vec3(0, 1, 0)));
	glm::mat3 ucm = { 0,-axis.z, axis.y,
						axis.z, 0, -axis.x,
						-axis.y, axis.x, 0 };
	glm::mat3 tensorprodU = { axis.x*axis.x, axis.x*axis.y, axis.x*axis.z,
								axis.y*axis.x, axis.y*axis.y, axis.y*axis.z,
								axis.z*axis.x, axis.z*axis.y, axis.z*axis.z };

	glm::mat3 tempMatrix = cosf(rad)* glm::mat3() + sinf(rad)*ucm + (1 - cosf(rad))*tensorprodU;
	glm::mat4 rotateAroundZ = glm::mat4(
		glm::vec4(tempMatrix[0], 0),
		glm::vec4(tempMatrix[1], 0),
		glm::vec4(tempMatrix[2], 0),
		glm::vec4(0, 0, 0, 1)
	);

	if (glm::dot(glm::vec3(rotateAroundZ * glm::vec4(mLookat, 1)), tempLookat) > 0) {
		this->mLookat = glm::vec3(rotateAroundZ * glm::vec4(mLookat, 1));
	}

	//mLookat = glm::rotate(glm::quat(cosf(mRot),0,sinf(mRot),0), glm::vec3(0,1));

	//this->mRotation.z = 
	this->mLookat = glm::vec3(rotatematrix * glm::vec4(mLookat, 1));
	mWorld = rmat * glm::scale(mWorld, glm::vec3(0.1f, 0.1f, 0.1f));
	mWorld[3][0] = mPosition.x;
	mWorld[3][1] = mPosition.y;
	mWorld[3][2] = mPosition.z;
	mWorld[3][3] = 1.f;
}
glm::vec3 Player::tacticalUpdate(const Input * inputs, const float &dt, const GameData &gameData)
{
	glm::vec3 dir(0.0f, 0.0f, 0.0f);	
	if (inputs->keyDown(SDLK_w))
	{
		dir += glm::vec3(0.0f, 0.0f, -10.0 * dt);
	}
	if (inputs->keyDown(SDLK_s))
	{
		dir += glm::vec3(0.0f, 0.0f, 10.0 * dt);
	}
	if (inputs->keyDown(SDLK_a))
	{
		dir += glm::vec3(-10.0 * dt, 0.0f, 0.0f);
	}
	if (inputs->keyDown(SDLK_d))
	{
		dir += glm::vec3(10.0 * dt, 0.0f, 0.0f);
	}
	return dir;
}

//void Player::render(const GLuint & programID, const glm::mat4 &viewMat)

void Player::render( GLuint worldLocation, GLuint animationLocation )
{
	GameObject::renderAni(worldLocation, animationLocation);
}
void Player::renderArrows(GLuint worldLocation, GLuint tintLocation) {
	this->mWeapon.render(worldLocation, tintLocation);
}

void Player::setAlive(bool amIalive)
{
	mHealth = 100;
}

glm::vec3 Player::getMovingDirection(glm::vec3 v1, glm::vec3 v2) {
	glm::vec3 result = glm::normalize(v1 + v2);
	if (result != result)
 		result = glm::vec3(0,0,0);
	return result;
}

bool Player::checkMove(glm::vec3 coord) {
	mBB.center = coord;

	bool intersect = false;
	for (int i = -1; i <= 1 && !intersect; i++) {
		for (int j = -1; j <= 1 && !intersect; j++) {
			int x = (int)((int)(coord.x + 1.0f) / pGameData->boxScale) + i;
			int y = (int)((int)(coord.z + 1.0f) / pGameData->boxScale) + j;
			if (!(y < 0 || x < 0 || y >= (int)pGameData->pGrid->getHeight() || x >= (int)pGameData->pGrid->getWidth())) {
				uchar b = pGameData->pGrid->getTile(x, y);
				if (b != TILE_BLOCKED && b != TILE_EMPTY) {
					if (this->mBB.intersect(glm::vec3(x*pGameData->boxScale, 0.5f, y*pGameData->boxScale), pGameData->boxScale / 2)) {
						intersect = true;
					}
				}
			}
		}
	}

	if( !intersect )
	{
		int w = pGameData->pGrid->getWidth();
		int h = pGameData->pGrid->getHeight();

		intersect = ( coord.x < 0.0f || coord.x >= (w-1)*pGameData->boxScale || coord.z < 0.0f || coord.z >= (h-1)*pGameData->boxScale);
	}
	return !intersect;
}

void Player::takeDamage(int damage) 
{
	this->mHealth -= damage;

	glm::vec3 pos = mPosition + glm::vec3( 0, 1, 0 );

	mEmitter.spawn(pos, glm::vec3(0, -1.0f, 0), 1.0f, 0.5f, glm::vec2( 0.5f ), glm::vec2( 0.4f ) );
	mEmitter.spawn(pos, glm::vec3(1.0f, -0.5f, 0), 1.0f, 0.5f, glm::vec2(0.5f), glm::vec2(0.4f));
	mEmitter.spawn(pos, glm::vec3(0, -0.5f, 1.0f), 1.0f, 0.5f, glm::vec2(0.5f), glm::vec2(0.4f));
	mEmitter.spawn(pos, glm::vec3(0, -0.5f, -1.0f), 1.0f, 0.5f, glm::vec2(0.5f), glm::vec2(0.4f));
	mEmitter.spawn(pos, glm::vec3(-1.0f, -0.5f, 0.0f), 1.0f, 0.5f, glm::vec2(0.5f), glm::vec2(0.4f));

	if (isRunningAttacking)
	{
		mAnimator.push(5, false, mAnimationSpeed, 1);
	}
	if (isBackAttacking)
	{
		mAnimator.push(12, false, mAnimationSpeed, 1);
	}
	if (isLStrafeAttacking)
	{
		mAnimator.push(25, false, mAnimationSpeed, 1);
	}
	if (isRStrafeAttacking)
	{
		mAnimator.push(31, false, mAnimationSpeed, 1);
	}
	if (isRunning)
	{
		mAnimator.push(7, false, mAnimationSpeed, 1);
	}
	if (isRunningHold)
	{
		mAnimator.push(7, false, mAnimationSpeed, 1);
	}
	if (isStrafingRight)
	{
		mAnimator.push(33, false, mAnimationSpeed, 1);
	}
	if (isStrafingLeft)
	{
		mAnimator.push(27, false, mAnimationSpeed, 1);
	}
	if (isBacking)
	{
		mAnimator.push(14, false, mAnimationSpeed, 1);
	}
}

bool Player::isAlive() {
	return this->mHealth > 0;
}

int Player::getHealth() const
{
	return mHealth;
}

Player::Player() : PI(glm::pi<float>())
{}

Player::Player(GameData* data, Emitter* smokeEmitter, Emitter* bloodEmitter) : GameObject(), PI(glm::pi<float>())
{
	this->pGameData = data;
	//mWeapon = new Weapon(true, data);
	mWeapon.load( data, true, smokeEmitter);
	mPosition = glm::vec3( 1, 1, 1 );
	yoffset = -0.5f;
	mWorld = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, yoffset, 1, 1 };
	mMaxSpeed = MOVESPEED;
	mAnimationSpeed = ANIMSPEED;
	speedY = 0;
	rotX = glm::pi<float>() * -0.5f;
	mStrength = 0.0f;
	mHealth = 100;
	daIndex = 0;
	state = 1;
	//setScale( 0.1f );
	mEmitter = *bloodEmitter;
	mCounter = 0;
	attackReadyTimer = 10.f;
	idleAttackTimer = 2.f;
	animationTimer = 0.f;
	stateRunning = false;
	enteredStates = false;

	jumpTimer = 0;
	isIdle = false;
	isHoldIdle = false;
	isJumping = false;
	isRunning = false;
	isRunningHold = false;
	isStrafingRight = false;
	isBacking = false;
	isBackingHold = false;
	isBackAttacking = false;
	isRStrafeAttacking = false;
	isRunningAttacking = false;
	isIdleAttacking = false;

	wasWDown = false;
	wasADown = false;
	wasSDown = false;
	wasDDown = false;
	wasRCDown = false;
}

Player::~Player()
{
	//delete mWeapon;
}