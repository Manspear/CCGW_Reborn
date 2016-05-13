#pragma once
#include "GameObject.h"
#include "Weapon.h"
#include "GameData.h"
#include "Grid.h"

class Player : public GameObject{
public:
	void update(const Input* inputs, const float &dt);
	glm::vec3 tacticalUpdate(const Input* inputs, const float &dt, const GameData &gameData);
	glm::vec3 getMovingDirection(glm::vec3 v1, glm::vec3 v2);
	glm::vec3 getLookAt() const;
	float getRot() const;
	void render(const GLuint & programID, const glm::mat4 &viewMat);
	bool checkMove(glm::vec3 coord);
	void setAlive(bool amIalive);
	void takeDamage(float damage);
	bool isAlive();
	float getYOffset();
	Weapon mWeapon;
	
	Player();
	Player(GameData* data, Emitter* emitter);
	~Player();
private: 
	float yoffset;
	float speedY;
	float mMaxSpeed;
	float mSpeed;
	float mStrength;
	glm::vec3 mDirection;
	GameData* pGameData;
	int daIndex;
	float mHealth;

	float mRot;
 };