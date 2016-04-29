#pragma once

#include <SDL\SDL.h>
#include <glm\glm.hpp>

enum
{
	MBUTTON_LEFT = 0,
	MBUTTON_RIGHT,
	MBUTTON_MIDDLE,
	MAX_BUTTONS,
};

#define MAX_KEYS 128

class Input
{
public:
	/* Polls SDL for events and updates the internal state if the input handler.*/
	bool update();

	/* Returns true if the key is currently down. Use the SDLK_ enum as parameter.*/
	bool keyDown(int key) const;
	/* Returns true if the key is currently up. Use the SDLK_ enum as parameter.*/
	bool keyUp(int key)const;
	/* Returns true if the key is currently down and previously up. Use the SDLK_ enum as parameter.*/
	bool keyPressed(int key) const;
	/* Returns true if the key is currently up and previously down. Use the SDLK_ enum as parameter.*/
	bool keyReleased(int key)const;

	/* Returns true if the button is currently down. Use MBUTTON_ enum as parameter.*/
	bool buttonDown(int button)const;
	/* Returns true if the button is currently up. Use MBUTTON_ enum as parameter.*/
	bool buttonUp(int button)const;
	/* Returns true if the button is currently down and previously up. Use MBUTTON_ enum as parameter.*/
	bool buttonPressed(int button)const;
	/* Returns true if the button is currently up and previously down. Use MBUTTON_ enum as parameter.*/
	bool buttonReleased(int button)const;

	/* Returns the current mouse position.*/
	glm::vec2 mousePosition() const;
	/* Returns the difference in mouse position from the last frame.*/
	glm::vec2 mouseDelta() const;

	bool toggleMouseLock();
	bool toggleMouseVisibility();

	void setMouseLock( bool enabled );
	void setMouseVisible( bool visible );

	bool getMouseLock() const;
	bool getMouseVisible() const;

	Input& operator=(const Input& ref);
	Input(const Input& ref);
	Input(SDL_Window* w);
	~Input();

private:
	bool mCurKeys[MAX_KEYS];
	bool mPrevKeys[MAX_KEYS];
	bool mCurButtons[MAX_BUTTONS];
	bool mPrevButtons[MAX_BUTTONS];
	bool mMouseLock, mMouseVisible;
	glm::vec2 mCurMouse;
	glm::vec2 mPrevMouse;

	SDL_Window* mpWindow;
};