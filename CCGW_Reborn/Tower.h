#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"
#include "Moleratman.h"


class Tower : public GameObject
{
public:
	bool load( GameData* data, glm::vec3 position, Model* model );
	void update(GameData* gameData, const float &dt);
	void render(const GLuint &programID);

	void setAlive( bool alive );
	bool getAlive() const;

	//Tower(GameData* gameData, glm::vec3 position, const Tower &towerRef, float scale);
	Tower();
	~Tower();
private:
	float mFireRate;
	float mReloadTime;
	float mRange;
	//Weapon* mpWeapon;
	Weapon mWeapon;
	bool mWeaponReady;
	bool mShooting;
	float mDistanceToTarget;
	float mStrength;
	Moleratman* targetEnemy;
	bool mAlive;
	
	bool arrowShot(const float &dt, GameData* data);
};