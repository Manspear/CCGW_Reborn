#pragma once
#include "GameObject.h"
#include "Weapon.h"
#include "GameData.h"
#include "Grid.h"

class Player : public GameObject{
public:
	void update(const Input* inputs, const float &dt);
	//void render(const GLuint & programID, const glm::mat4 &viewMat);
	void render( GLuint worldLocation, GLuint animationLocation );

	glm::vec3 tacticalUpdate(const Input* inputs, const float &dt, const GameData &gameData);
	glm::vec3 getMovingDirection(glm::vec3 v1, glm::vec3 v2);
	glm::vec3 getLookAt() const;
	float getRot() const;
	float getStrength() const;
	bool checkMove(glm::vec3 coord);
	void setAlive(bool amIalive);
	void takeDamage(int damage);
	bool isAlive();
	int getHealth() const;
	void setHealth(int health);
	float getYOffset();
	Weapon mWeapon;
	void renderArrows(GLuint worldLocation, GLuint tintLocation);
	
	Player();
	Player(GameData* data, Emitter* smokeEmitter, Emitter* bloodEmitter);
	~Player();
private: 
	#define ATKRDYTIMER 10
	#define ANIMSPEED 1.5
	#define MOVESPEED 7

	enum
	{
		ANIM_RUN = 1,
		ANIM_RELOAD,
		ANIM_SHOOT,
	};

	struct sStates
	{
		int state1[5];
		int state2[5];
		int state3[5];
		int state4[5];
		int state5[5];
		int state6[5];
	};
	/**
	0 = run
	1 = back
	2 = idle
	3 = left
	4 = right

	blend only when going from state1(stand) to state2(attack)

	state1 = normal run/stand
	state2 = halfattack
	state3 = frame 17 (waiting for you to release)
	state4 = halfattack the rest of it
	state5 = hold (waiting for attack again)
	state6 = takedamage

	pressed
	hold
	release
	
	if(leftMouseButton.event) 
		{
		if(state == 1) normal run/stand
		{
			if(!animRun)
			{
				float dt;
				starta dt;
				if(w)
				{
					animRun = true;
					play.runAnimation(1,0,0,1);// run_blend
					play.runAnimation(2,0,0,1);// run_attack
					state = 2
				}
				else if(s)
				{
					play.backAnimation(1,0,0,1);// back_blend
					play.backAnimation(2,0,0,1);// back_attack
					state = 2
				}
				else if(a)
				{
					play.leftAnimation(1,0,0,1);// left_blend
					play.leftAnimation(2,0,0,1);//  left_attack
					state = 2
				}
				else if(d)
				{
					play.rightAnimation(1,0,0,1);// right_blend
					play.rightAnimation(2,0,0,1);// right_attack
					state = 2
				}

				if(dt >= 1.5 && leftMouseButton.hold)
				{
					state = 3;
				}
			}
		}
		else if(state == 2) halfattack
		{	
			if(w)
			{
				play.runAnimation(3,0,0,1);// run_attack_17
			}
			else if(s)
			{
				play.backAnimation(3,0,0,1);// back_attack_17
			}
			else if(a)
			{
				play.leftAnimation(3,0,0,1);// left_attack_17
			}
			else if(d)
			{
				play.rightAnimation(3,0,0,1);// right_attack_17
			}
			state == 3;
			if(leftMouseButton.release)
			{
				state = 4; halfattack the rest of it
			}
		}
		else if(state == 3) frame 17 (waiting for you to release)
		{
			if(w)
			{
				play.runAnimation(3,0,0,1);// run_attack_17
			}
			else if(s)
			{
				play.backAnimation(3,0,0,1);// back_attack_17
			}
			else if(a)
			{
				play.leftAnimation(3,0,0,1);// left_attack_17
			}
			else if(d)
			{
				play.rightAnimation(3,0,0,1);// right_attack_17
			}
			if(leftMouseButton.release)
			{
			state = 4; halfattack the rest of it
			}
		}
		else if(state == 4) halfattack the rest of it
		{
			float dt;
			starta dt;
			if(w)
			{
				play.runAnimation(2,0,0,1);// run_attack från frame 17 till 32
			}
			else if(s)
			{
				play.backAnimation(2,0,0,1);// back_attack från frame 17 till 32
			}
			else if(a)
			{
				play.leftAnimation(2,0,0,1);// left_attack från frame 17 till 32
			}
			else if(d)
			{
				play.rightAnimation(2,0,0,1);// right_attack från frame 17 till 32
			}
			state == 4; halfattack the rest of it

			if(dt >= 1.5 && leftMouseButton.keyDown)// kollar om knappen är intryckt
			{
				state == 2;// attack again
			}
			else if(dt >= 1.5 && !leftMouseButton.release)// kollar om den är i release state på boolen
			{
				play.runAnimation(4,0,0,1);// run_hold
					state == 5;// hold mode
			}
		}
		else if(state == 5) hold (waiting for attack again)
		{	
			if(w)
			{
				play.runAnimation(2,0,0,1);// run_attack
			}
			else if(s)
			{
				play.backAnimation(2,0,0,1);// back_attack
			}
			else if(a)
			{
				play.leftAnimation(2,0,0,1);// left_attack
			}
			else if(d)
			{
				play.rightAnimation(2,0,0,1);// right_attack
			}
			if(dt >= 1.5 && leftMouseButton.hold)
			{
				state = 2;
			}
		}
		else if(state == 6) takedamage
		{
			if(w)
			{
				play.runAnimation(5,0,0,1);// run_takedamage
			}
			else if(s)
			{
				play.backAnimation(5,0,0,1);// back_takedamage
			}
			else if(a)
			{
				play.leftAnimation(5,0,0,1);// left_takedamage
			}
			else if(d)
			{
				play.rightAnimation(5,0,0,1);// right_takedamage
			}
		}
	}
	else // vanlig run
	{
		if(w)
		{
			play.runAnimation(5,0,0,1);// run
		}
		else if(s)
		{
			play.backAnimation(5,0,0,1);// back
		}
		else if(a)
		{
			play.leftAnimation(5,0,0,1);// left
		}
		else if(d)
		{
			play.rightAnimation(5,0,0,1);// right
		}
	}

	**/
	sStates animState;

	bool isJumping;
	bool isRunning;
	bool isRunningHold;
	bool isStrafingRight;
	bool isStrafingLeft;
	bool isBacking;
	bool isBackingHold;
	bool isIdle;
	bool isHoldIdle;
	bool wasWDown;
	bool wasADown;
	bool wasSDown;
	bool wasDDown;
	bool wasRCDown;

	bool stateRunning;
	bool enteredStates;

	bool isIdleAttacking;
	bool isRunningAttacking;
	bool isRStrafeAttacking;
	bool isLStrafeAttacking;
	bool isBackAttacking;

	int state;

	float jumpTimer;
	float mAnimationSpeed;
	float attackReadyTimer;
	float yoffset;
	float speedY;
	float mMaxSpeed;
	float mSpeed;
	float mStrength;

	float animationTimer;
	float idleAttackTimer;

	const float PI;
	glm::vec3 mDirection;
	GameData* pGameData;
	int daIndex;
	int mHealth;

	float mCounter;

	Emitter mEmitter;

	float mRot;
 };