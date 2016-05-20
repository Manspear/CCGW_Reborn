#include "Sound.h"

//
// Sound
//
bool Sound::load( Assets* assets, string file)
{
	if( !gHasAudio )
		return false;

	mpChunk = Mix_LoadWAV( file.c_str() );
	return ( mpChunk != nullptr );
}

void Sound::unload()
{
	if( mpChunk )
		Mix_FreeChunk( mpChunk );
}

void Sound::play()
{
	if( gHasAudio )
		Mix_PlayChannel( mChannel, mpChunk, mLooping );
}

void Sound::pause()
{
	if( gHasAudio )
		Mix_Pause( mChannel );
}

void Sound::resume()
{
	if( gHasAudio )
		Mix_Resume( mChannel );
}

void Sound::stop()
{
	if (gHasAudio)
		Mix_HaltChannel( mChannel );
}

int Sound::setVolume( int volume )
{
	if (gHasAudio)
		return Mix_VolumeChunk( mpChunk, volume );
}

void Sound::setChannel( int channel )
{
	mChannel = channel;
}

void Sound::setLooping( bool looping )
{
	if (this != nullptr)
		mLooping = looping;
}

int Sound::getChannel() const
{
	return mChannel;
}

bool Sound::getLooping() const
{
	return mLooping;
}

bool Sound::getPaused() const
{
	return Mix_Paused( mChannel );
}

Sound& Sound::operator=(const Sound& ref)
{
	mpChunk = ref.mpChunk;
	mChannel = ref.mChannel;
	mLooping = ref.mLooping;
	return *this;
}

Sound::Sound(const Sound& ref)
	: mpChunk( ref.mpChunk ), mChannel( ref.mChannel ), mLooping( ref.mLooping )
{
}

Sound::Sound()
	: mpChunk( nullptr ), mChannel( -1 ), mLooping( false )
{
}

Sound::~Sound()
{
}

//
// Music
//
bool Music::load( Assets* assets, string file )
{
	if (!gHasAudio)
		return false;

	mpMusic = Mix_LoadMUS( file.c_str() );
	return ( mpMusic != nullptr );
}

void Music::unload()
{
	if( mpMusic )
		Mix_FreeMusic( mpMusic );
}

void Music::play()
{
	if (gHasAudio)
		Mix_PlayMusic( mpMusic, mLooping );
}

void Music::pause()
{
	if (gHasAudio)
		Mix_PauseMusic();
}

void Music::stop()
{
	if (gHasAudio)
		Mix_HaltMusic();
}

int Music::setVolume( int volume )
{
	return Mix_VolumeMusic( volume );
}

void Music::setLooping( bool looping )
{
	mLooping = looping;
}

bool Music::getLooping() const
{
	return mLooping;
}

bool Music::getPaused() const
{
	return Mix_PausedMusic();
}

Music& Music::operator=( const Music& ref )
{
	mpMusic = ref.mpMusic;
	return *this;
}

Music::Music( const Music& ref )
	: mpMusic( ref.mpMusic )
{
}

Music::Music()
	: mpMusic( nullptr )
{
}

Music::~Music()
{
}