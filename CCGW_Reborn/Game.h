#pragma once
#include "ActionState.h"
#include "DeferredProgram.h"
#include "ForwardProgram.h"
#include "tempMesh.h"
#include "Mesh.h"
#include "Player.h"
#include "Input.h"
#include "Camera.h"
#include "global_variables.h"
#include "Terrain.h"
#include "Texture.h"
#include "Billboard.h"
#include "Emitter.h"
#include "Grid.h"
#include "Enemy.h"
#include "Marker.h"
#include "Tower.h"
#include "WaveSpawner.h"
#include "Molebat.h"
#include "Moleratman.h"
#include "Sound.h"

enum State {
	GAME_LOST, GAME_WON, GAME_PLAYING
};

class Game {
public:
	bool tactical;



	State run(Input* inputs, const float &dt, bool menuActive);
	void tacticalRun(Input* inputs, const float &dt, bool menuActive);

	Game();
	~Game();
private:
	GameState* pActionState;
	GLuint testScreen;
	Terrain mGround;
	Marker mActionMarker;
	Marker mTacticalMarker;
	Tower mTowerModel;
	GameData data;
	sNode* mpPath;
	int mTargets;
	//std::vector<Tower*> mpTowers;
	WaveSpawner* pWaveSpawner;

	int mGold;
	int mScore;

	int mMaxWaves;
	int mBabyCount;


	float mCounter;
	float mDelayCleared;
	float mDelay2;


	void createScreenQuad();
	void drawOnScreenQuad();
	void render();
	void buildTowers();
	void update(Input* inputs, float dt);
};
