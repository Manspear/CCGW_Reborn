#include <GL\glew.h>
#include <iostream>
#include <Windows.h>
#include <SDL\SDL.h>
#include <SDL\SDL_mixer.h>
#include "Game.h"
#include "Input.h"
#include "global_variables.h"
#include "Menu.h"

using namespace std;

int main(int argc, char** argv) 
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	SDL_Init(SDL_INIT_EVERYTHING);
	Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
	SDL_Window* window = nullptr;

	bool running = true;

	SDL_DisplayMode dm;
	SDL_GetDesktopDisplayMode(0, &dm);
	//gWidth = dm.w - 2;
	//gHeight = dm.h - 60;
	gWidth = 1080 - 60;
	gHeight = 720-2;

	window = SDL_CreateWindow("Try hard!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, gWidth, gHeight, SDL_WINDOW_OPENGL); // can get the border & titlebar sizes to fix a more precise windowed "fullscreen"
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	if (glContext == nullptr)
		std::cout << "SDLFEL";
	GLenum error = glewInit();
	if (error != GLEW_OK)
		std::cout << "GlewFel!";
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	glClearColor(0.7f, 0.7f, 1.0f, 1.0);
	glEnable(GL_DEPTH_TEST);

	Input input(window);
	Game game;

	SDL_GL_SetSwapInterval( 1 );

	float dt = 0;
	int timeStamp = SDL_GetTicks(); int temp;
	bool actionMode = true;
	input.setMouseVisible(true);
	input.setMouseLock(false);

	State gs = GAME_PLAYING;

	int fps = 0;
	float fpsTimer = 0.0f;

	Menu mainMenu;
	char menuAction = 0;
	while (running)
	{
		temp = SDL_GetTicks();
		dt = (float)(temp - timeStamp) / 1000.f;
		timeStamp = temp;
		input.update();

		fpsTimer += dt;
		fps++;
		if( fpsTimer > 1.0f )
		{
			std::cout << "FPS: " << fps << std::endl;
			fps = 0;
			fpsTimer -= 1.0f;
		}
		
		if (!game.tactical)
			gs = game.run(&input, dt, !mainMenu.mActive);
		else
			game.tacticalRun(&input, dt, !mainMenu.mActive);
		running = mainMenu.update(&input, game.getGameData(), gs);
		mainMenu.render();
		
		SDL_GL_SwapWindow(window);
	}
	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}