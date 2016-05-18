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
#include "Model.h"

enum State {
	GAME_LOST, GAME_WON, GAME_PLAYING
};

class Game {
public:
	bool tactical;

	GameData* getGameData();
	void restartGame();

	State run(Input* inputs, const float &dt, bool menuActive);
	void tacticalRun(Input* inputs, const float &dt, bool menuActive);

	Game();
	~Game();
private:
	GameState* pActionState;
	GLuint testScreen;
	Terrain mGround;
	Marker mTacticalMarker;
	GameData data;
	int mTargets;
	WaveSpawner* pWaveSpawner;
	Tower** mpVisibleTowers;
	int mVisibleTowers;

	int mGold;
	int mScore;

	int mMaxWaves;

	float mCounter;
	float mDelayCleared;
	float mDelay2;


	void createScreenQuad();
	void drawOnScreenQuad();
	void render();
	void update(Input* inputs, float dt);
};
