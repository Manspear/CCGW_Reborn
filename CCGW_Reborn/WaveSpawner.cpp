#include "WaveSpawner.h"
#include <random>

void WaveSpawner::update( float deltaTime )
{
	mDelay -= deltaTime;
	if( mDelay <= 0.0f )
	{
		if( mCurMoleratmen < mSpawnMoleratmen && spawnSwitch == false)
		{
			mCurMoleratmen++;
			spawnMoleratman();
			spawnSwitch = true;
		}
		else if (mCurMolebats < mSpawnMolebats && spawnSwitch == true)
		{
			mCurMolebats++;
			spawnMolebat();
			spawnSwitch = false;
		}
		else if (spawnSwitch == false)
		{
			spawnSwitch = true;
		}
		else if (spawnSwitch == true)
		{
			spawnSwitch = false;
		}
		
		mDelay = mSpawnDelay;
	}
}

void WaveSpawner::spawn()
{
	// start by getting the path for the moleratmen
	sNode start = { 7, 0 };
	sNode end = { 7 , 47 };
	pGameData->pGrid->findPath(start, end, mpPath, &mTargets);
	
	//if( pGameData->pGrid->findPath( start, end, mpPath[i], &mTargets ) )
	//{
	incrementWave();
	
	mCurMoleratmen = 0;
	mCurMolebats = 0;

	mDelay = 0.0f;

	// TODO: Do something useful here
	/*
	mSpawnMoleratmen = glm::min((mWave*2 + 3), pGameData->mMoleratmen);
	mSpawnMolebats = glm::min((mWave + 1), pGameData->mMolebats);
*/
	

	if (mWave <= 3)
	{
		if (waveLevel[0] == true)
		{
			mMoleRatWaveSize = 0.f;
			mMoleBatWaveSize = 0.f;
			mSpawnDelay = 0.4f;
			waveLevel[0] = false;
		}
		mMoleRatWaveSize += 5;
		mMoleBatWaveSize += 3;

		mRatHP = 5.f;
		mBatHP = 4.f;
		mRatSpeed = 4.f;
		mBatSpeed = 4.f;
	}
	if (mWave > 3 && mWave <= 8)
	{
		if (waveLevel[1] == true)
		{
			mMoleRatWaveSize = 5;
			mMoleBatWaveSize = 2;
			mSpawnDelay = 0.3f;
			waveLevel[1] = false;
		}
		mMoleRatWaveSize += 5;
		mMoleBatWaveSize += 3;

		mRatHP = 10.f;
		mBatHP = 5.f;
		mRatSpeed = 5.f;
		mBatSpeed = 5.f;
	}
	if (mWave > 8)
	{
		if (waveLevel[2] == true)
		{
			mMoleRatWaveSize = 10.f;
			mMoleBatWaveSize = 4.f;
			mSpawnDelay = 0.1f;
			waveLevel[2] = false;
		}
		mMoleRatWaveSize += 5;
		mMoleBatWaveSize += 3;
		mRatHP = 15.f;
		mBatHP = 7.f;
		mRatSpeed = 7.f;
		mBatSpeed = 7.f;
	}
	mSpawnMoleratmen = mMoleRatWaveSize;
	mSpawnMolebats = mMoleBatWaveSize;
	
	playSound(mWave - 1);
}


bool WaveSpawner::hasWon() {
	return this->mWave > 18;
}

void WaveSpawner::incrementWave()
{
	mWave++;
}

void WaveSpawner::loadSound(Sound* sound, int i)
{
	if (sound != nullptr)
		pWaveAnnounce[i] = sound;
}

void WaveSpawner::setPosition( glm::vec3 position )
{
	mPosition = position;
}

void WaveSpawner::restart()
{
	mWave = 0;  mDelay = 0; mCurMoleratmen = 0; mSpawnMoleratmen = 0;
	mCurMolebats = 0; mSpawnMolebats = 0;  mMoleratmanIndex = 0; mMolebatIndex = 0;
	mTargets = 0;
	for (int i = 0; i < waveLevel.size(); i++)
	{
		waveLevel[i] = true;
	}
	mMoleRatWaveSize = STARTRATWAVESIZE;
	mMoleBatWaveSize = STARTBATWAVESIZE;
	spawnSwitch = false;
}

glm::vec3 WaveSpawner::getPosition() const
{
	return mPosition;
}

int WaveSpawner::getWave() const
{
	return mWave;
}

void WaveSpawner::spawnMoleratman()
{
	Moleratman* m = nullptr;

	// NOTE: Avoid linear congestion. If we always check from the start of the array
	// then we will be wasting more time for every moleratman we spawn.
	// By remembering the last index we checked, the next index is less likely to be in use.
	for( int i=0; i<pGameData->mMoleratmen && m == nullptr; i++, mMoleratmanIndex++ )
	{
		if( mMoleratmanIndex >= pGameData->mMoleratmen )
			mMoleratmanIndex = 0;

		if( !pGameData->pMoleratmen[mMoleratmanIndex].getAlive() )
			m = &pGameData->pMoleratmen[mMoleratmanIndex];
	}

	if( m )
	{
			m->setPosition(mPosition);
			m->setAlive(true);
			m->setLife(mRatHP);
			m->setSpeed(mRatSpeed);
			m->setPath(mpPath, mTargets);
	}
}

void WaveSpawner::spawnMolebat()
{
	Molebat* m = nullptr;

	for( int i=0; i<pGameData->mMolebats && m == nullptr; i++, mMolebatIndex++ )
	{
		if( mMolebatIndex > pGameData->mMolebats )
			mMolebatIndex = 0;

		if( !pGameData->pMolebats[mMolebatIndex].getAlive() )
			m = &pGameData->pMolebats[mMolebatIndex];
	}

	if( m )
	{
		glm::vec3 pos = mPosition;
		pos.y = MOLEBAT_HEIGHT;
		m->setPosition( pos );
		m->setAlive( true );
		m->setLife( mBatHP );
		m->setSpeed(mBatSpeed);
	}
}

void WaveSpawner::playSound(int index)
{
	if (pWaveAnnounce[index] != nullptr)
		pWaveAnnounce[index]->play();
}

WaveSpawner& WaveSpawner::operator=( const WaveSpawner& ref )
{
	mWave = ref.mWave;
	mDelay = ref.mDelay;
	mPosition = ref.mPosition;

	mCurMoleratmen = ref.mCurMoleratmen;
	mSpawnMoleratmen = ref.mSpawnMoleratmen;
	mCurMolebats = ref.mCurMolebats;
	mSpawnMolebats = ref.mSpawnMolebats;

	// TODO: This shouldn't be necessary
	pGameData = ref.pGameData;

	return *this;
}

WaveSpawner::WaveSpawner( const WaveSpawner& ref )
	: pGameData( ref.pGameData ),
	mWave( ref.mWave ), mDelay( ref.mDelay ), mPosition( ref.mPosition ),
	mCurMoleratmen( ref.mCurMoleratmen ), mSpawnMoleratmen( ref.mSpawnMoleratmen ),
	mCurMolebats( ref.mCurMolebats ), mSpawnMolebats( ref.mSpawnMolebats ),
	mMoleratmanIndex( ref.mMoleratmanIndex ), mMolebatIndex( ref.mMolebatIndex ),
	mTargets( ref.mTargets )
{
		mpPath = new sNode[20*20];
}

WaveSpawner::WaveSpawner(GameData* data)
	: pGameData(data),
	mWave(0), mDelay(0.0f), mPosition(0.0f) ,
	mCurMoleratmen( 0 ), mSpawnMoleratmen( 0 ),
	mCurMolebats( 0 ), mSpawnMolebats( 0 ),
	mMoleratmanIndex( 0 ), mMolebatIndex( 0 ),
	mTargets( 0 )
{
	mpPath = new sNode[20 * 20];
	mMoleRatWaveSize = STARTRATWAVESIZE;
	mMoleBatWaveSize = STARTBATWAVESIZE;
	mSpawnDelay = 1.f;
	spawnSwitch = false;

	waveLevels = 3;
	for (int i = 0; i < waveLevels; i++)
	{
		waveLevel.push_back(true);
	}
}

WaveSpawner::~WaveSpawner()
{
	delete mpPath;
}