#pragma once

#include "GameObject.h"
#include "Emitter.h"
#include "GameData.h"
#include "BoundingBox.h"

#define ARROW_MAX_PIERCING_DEPTH 3

class Enemy;
class Moleratman;
class Molebat;
class Tower;
class Arrow : public GameObject{
public:
	bool load(GameData* data, Model* model, Emitter* emitter );

	bool isAlive();
	void update(float dt);
	void spawn(bool owner, glm::vec3 position, glm::vec3 direction, float travelSpeed, glm::vec3 downVector, float rotation);
	Arrow();
	~Arrow();

protected:
	GameData* pGameData;

	bool mPlayerOwned;

	float mSpeed;
	glm::vec3 mGravitation;
	float mTimeSinceLastEmmit;
	float mEmmitInterval;

	glm::vec3 mVelocity;
	bool mAlive, mPlayerArrow, mPiercing;

	Enemy *mpPiercedEnemies[ARROW_MAX_PIERCING_DEPTH];
	int mPiercings;

	Emitter mEmitter;
};