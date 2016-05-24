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

	bool isIdleAttacking;
	bool isRunningAttacking;
	bool isRStrafeAttacking;
	bool isLStrafeAttacking;
	bool isBackAttacking;

	float jumpTimer;
	float mAnimationSpeed;
	float attackReadyTimer;
	float yoffset;
	float speedY;
	float mMaxSpeed;
	float mSpeed;
	float mStrength;
	const float PI;
	glm::vec3 mDirection;
	GameData* pGameData;
	int daIndex;
	int mHealth;

	float mCounter;

	Emitter mEmitter;

	float mRot;
 };