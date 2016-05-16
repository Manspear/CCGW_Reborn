#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"
#include "Moleratman.h"


class Tower : public GameObject
{
public:
	bool load( GameData* data, glm::vec3 position, Model* boxModel, Model* ballistaModel, Model* headModel, Emitter* emitter );
	void update(GameData* gameData, const float &dt);
	//void render(const GLuint &programID);
	void renderNonAni( GLuint worldLocation );
	void renderAni( GLuint worldLocation, GLuint animationLocation );
	void renderArrows( GLuint worldLocation );

	void setAlive( bool alive );
	void setHasBallista( bool hasBallista );
	bool getAlive() const;
	bool getHasBallista() const;
	Weapon mWeapon;
	//Tower(GameData* gameData, glm::vec3 position, const Tower &towerRef, float scale);
	Tower();
	~Tower();
private:
	float mFireRate;
	float mReloadTime;
	float mRange;
	bool mWeaponReady;
	bool mShooting;
	float mDistanceToTarget;
	float mStrength;
	Moleratman* targetEnemy;
	bool mAlive, mHasBallista;
	Model *mpBoxModel, *mpBallistaModel, *mpHeadModel;
	
	bool arrowShot(const float &dt, GameData* data);
};