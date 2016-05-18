#include "WaveSpawner.h"
#include <random>

void WaveSpawner::update( float deltaTime )
{
	mDelay -= deltaTime;

	if( mDelay <= 0.0f )
	{
		// TODO: Should we always spawn moleratmen before spawning molebats?
		// Maybe every other? Maybe random?
		if( mCurMoleratmen < mSpawnMoleratmen )
		{
			mCurMoleratmen++;
			spawnMoleratman();
		}
		else if (mCurMolebats < mSpawnMolebats)
		{
			mCurMolebats++;
			spawnMolebat();
		}
		mDelay = WAVESPAWNER_DELAY;
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
	mSpawnMoleratmen = mMoleRatWaveSize;
	mSpawnMolebats = mMoleBatWaveSize;

	mMoleRatWaveSize = mMoleRatWaveSize * 2.f;
	mMoleBatWaveSize = mMoleBatWaveSize * 2.f;

	if (mWave <= 8)
	{
		mRatHP = 50.f;
		mBatHP = 1.f;
		mRatSpeed = 4.f;
		mBatSpeed = 2.f;
	}
	if (mWave > 8 && mWave <= 16)
	{
		mRatHP = 100.f;
		mBatHP = 1.f;
		mRatSpeed = 6.f;
		mBatSpeed = 4.f;
	}
	if (mWave > 16)
	{
		mRatHP = 150.f;
		mBatHP = 1.f;
		mRatSpeed = 10.f;
		mBatSpeed = 8.f;
	}
	// DEBUG: Remove this
	//for( int i=0; i<pGameData->mMoleratmen; i++ )
	//	pGameData->pMoleratmen[i].setAlive( false );

	//for (int i = 0; i<pGameData->mMolebats; i++)
	//	pGameData->pMolebats[i].setAlive(false);
	//}
}


bool WaveSpawner::hasWon() {
	return this->mWave > 25;
}

void WaveSpawner::incrementWave()
{
	mWave++;
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
}

glm::vec3 WaveSpawner::getPosition() const
{
	return mPosition;
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
			m->setSpeed(mBatSpeed);
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
	}
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
	mMoleRatWaveSize = 5.f;
	mMoleBatWaveSize = 2.f;
}

WaveSpawner::~WaveSpawner()
{
	delete mpPath;
}