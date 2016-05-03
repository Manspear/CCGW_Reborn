#pragma once

#include "Moleratman.h"
#include "Molebat.h"

#define WAVESPAWNER_DELAY 1.0f

//Class for spawining waves of enemy.
//It keeps track of how many enemies to spawn and the delay between each enemy.
class WaveSpawner
{
public:
	//Spawns new enemies when enough time has passed.
	void update( float deltaTime );
	//Starts spawning new enemies.
	void spawn();
	//Adds 1 to the wave counter.
	void incrementWave();

	bool hasWon();

	void setPosition( glm::vec3 position );
	glm::vec3 getPosition() const;

	WaveSpawner& operator=( const WaveSpawner& ref );
	WaveSpawner( const WaveSpawner& ref );
	WaveSpawner( GameData* data );
	~WaveSpawner();

private:
	void spawnMoleratman();
	void spawnMolebat();

	GameData* pGameData;

	int mWave;
	float mDelay;
	glm::vec3 mPosition;

	int mCurMoleratmen, mSpawnMoleratmen;
	int mCurMolebats, mSpawnMolebats;
	int mMoleratmanIndex, mMolebatIndex;

	sNode* mpPath;
	int mTargets;
};