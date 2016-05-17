#pragma once
#include "GameObject.h"
#include "Enemy.h"
#include "Weapon.h"
#include "Player.h"
#include "Moleratman.h"

#define TOWER_MODELS 7

class Tower : public GameObject
{
public:
	bool load( GameData* data, glm::vec3 position, Model** models, Emitter* emitter );
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

	union
	{
		Model* mpModels[TOWER_MODELS];
		struct
		{
			Model *mpBaseModel, *mpLidModel, *mpCrossbowModel;
			Model *mpSmallCylinderModel, *mpMidCylinderModel;
			Model *mpLowWheelModel, *mpHighWheelModel;
		};
	};
	
	bool arrowShot(const float &dt, GameData* data);
};