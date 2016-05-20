#pragma once

#include "Moleratman.h"
#include "Molebat.h"

//Class for spawining waves of enemy.
//It keeps track of how many enemies to spawn and the delay between each enemy.
class WaveSpawner
{
public:
	#define STARTRATWAVESIZE 5
	#define STARTBATWAVESIZE 2
	//Spawns new enemies when enough time has passed.
	void update( float deltaTime );
	//Starts spawning new enemies.
	void spawn();
	//Adds 1 to the wave counter.
	void incrementWave();
	void loadSound(Sound* sound, int i);
	bool hasWon();
	void playSound(int index);

	void setPosition( glm::vec3 position );
	void restart();
	glm::vec3 getPosition() const;
	int getWave() const;

	WaveSpawner& operator=( const WaveSpawner& ref );
	WaveSpawner( const WaveSpawner& ref );
	WaveSpawner( GameData* data );
	~WaveSpawner();

private:
	void spawnMoleratman();
	void spawnMolebat();

	GameData* pGameData;

	int mWave;
	int mMoleRatWaveSize;
	int mMoleBatWaveSize;
	int waveLevels;
	float mRatHP;
	float mBatHP;
	float mRatSpeed;
	float mBatSpeed;
	float mDelay;
	float mSpawnDelay;
	std::vector<bool> waveLevel;

	bool spawnSwitch;
	glm::vec3 mPosition;

	int mCurMoleratmen, mSpawnMoleratmen;
	int mCurMolebats, mSpawnMolebats;
	int mMoleratmanIndex, mMolebatIndex;

	Sound* pWaveAnnounce[27];
	sNode* mpPath;
	int mTargets;
};