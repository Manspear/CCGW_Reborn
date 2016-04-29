#pragma once

#include <SDL\SDL_mixer.h>
#include <vector>
#include "Assets.h"

/*Wrapper class for loading and playing short sounds.
If channel is -1 (default) it will affect all sounds in the mixer.*/
class Sound : public Asset
{
public:
	/* Load a sound from a file. Returns true if the load was successful.*/
	bool load( std::string file );
	/* Unloads the sound. */
	void unload();
	/* Plays the sound. */
	void play();
	/* Pauses the sound. */
	void pause();
	/* Resumes the sound. */
	void resume();
	/* Stops the sound. */
	void stop();

	int setVolume( int volume );
	void setChannel( int channel );
	void setLooping( bool looping );

	int getChannel() const;
	bool getLooping() const;
	bool getPaused() const;

	Sound& operator=(const Sound& ref);
	Sound(const Sound& ref);
	Sound();
	~Sound();

private:
	Mix_Chunk* mpChunk;
	int mChannel;
	bool mLooping;
};

/*Wrapper class for loading and playing music.
Only one piece of music can be playing at any time.*/
class Music : public Asset
{
public:
	/* Load music from a file. Returns true if the load was successful. */
	bool load( std::string file );
	/* Unloads the music. */
	void unload();
	/* Plays the music. */
	void play();
	/* Pauses the music. */
	void pause();
	/* Stops the music. */
	void stop();

	int setVolume( int volume );
	void setLooping( bool looping );

	bool getLooping() const;
	bool getPaused() const;

	Music& operator=( const Music& ref );
	Music( const Music& ref );
	Music();
	~Music();

private:
	Mix_Music* mpMusic;
	bool mLooping;
};