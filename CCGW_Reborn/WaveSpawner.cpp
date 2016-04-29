#include "WaveSpawner.h"

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
		else if( mCurMolebats < mSpawnMolebats )
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
	sNode start = { 0, 0 }, end = { 10, 10 };
	if( pGameData->pGrid->findPath( start, end, mpPath, &mTargets ) )
	{
		incrementWave();

		mCurMoleratmen = 0;
		mCurMolebats = 0;

		mDelay = 0.0f;

		// TODO: Do something useful here
		mSpawnMoleratmen = mWave % pGameData->mMoleratmen;
		mSpawnMolebats = mWave % pGameData->mMolebats;

		// DEBUG: Remove this
		for( int i=0; i<pGameData->mMoleratmen; i++ )
			pGameData->pMoleratmen[i].setAlive( false );

		for (int i = 0; i<pGameData->mMolebats; i++)
			pGameData->pMolebats[i].setAlive(false);
	}
}

void WaveSpawner::incrementWave()
{
	mWave++;
}

void WaveSpawner::setPosition( glm::vec3 position )
{
	mPosition = position;
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
		m->setPosition( mPosition );
		m->setAlive( true );
		m->setPath( mpPath, mTargets );
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

WaveSpawner::WaveSpawner( GameData* data )
	: pGameData( data ),
	mWave( 0 ), mDelay( 0.0f ), mPosition( 0.0f ),
	mCurMoleratmen( 0 ), mSpawnMoleratmen( 0 ),
	mCurMolebats( 0 ), mSpawnMolebats( 0 ),
	mMoleratmanIndex( 0 ), mMolebatIndex( 0 ),
	mTargets( 0 )
{
	mpPath = new sNode[20*20];
}

WaveSpawner::~WaveSpawner()
{
	delete[] mpPath;
}