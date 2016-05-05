#include "Input.h"
#include "global_variables.h"

bool Input::update()
{
	bool result = true;

	// copy last frames current input to current frames previous input
	for (int i = 0; i<MAX_KEYS; i++)
		mPrevKeys[i] = mCurKeys[i];

	for (int i = 0; i<MAX_BUTTONS; i++)
		mPrevButtons[i] = mCurButtons[i];


	// poll SDL for input
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) // user pressed the X button of the window
			mQuit = true;
		else if (e.type == SDL_KEYDOWN) // user pressed a key
		{
			int key = e.key.keysym.sym;
			if (key >= 0 && key < MAX_KEYS)
				mCurKeys[key] = true;
		}
		else if (e.type == SDL_KEYUP) //  user released a key
		{
			int key = e.key.keysym.sym;
			if (key >= 0 && key < MAX_KEYS)
				mCurKeys[key] = false;
		}
		else if (e.type == SDL_MOUSEBUTTONDOWN) // user pressed a mouse button
		{
			int button = e.button.button - 1; // SDL_BUTTON_LEFT is 1, we want it to be 0
			if (button >= 0 && button < MAX_BUTTONS)
				mCurButtons[button] = true;
		}
		else if (e.type == SDL_MOUSEBUTTONUP) // user released a mouse button
		{
			int button = e.button.button - 1;
			if (button >= 0 && button < MAX_BUTTONS)
				mCurButtons[button] = false;
		}
		
		else if (e.type == SDL_MOUSEMOTION) 
		{
			this->mPrevMouse = mCurMouse;
			this->mCurMouse = glm::vec2(e.motion.x, e.motion.y);
		}
	}

	int tempx = 0; int tempy = 0; 
	SDL_GetWindowPosition( mpWindow, &tempx, &tempy );
	this->mPrevMouse =glm::vec2(gWidth/2, gHeight/2);

	if( mMouseLock )
	{
		SDL_WarpMouseInWindow( mpWindow, gWidth/2, gHeight/2 );
		SDL_FlushEvent(SDL_MOUSEMOTION);
	}

	return result;
}

bool Input::keyDown(int key) const
{
	if (key < 0 || key >= MAX_KEYS)
		return false;
	return mCurKeys[key];
}

bool Input::keyUp(int key)const
{
	if (key < 0 || key >= MAX_KEYS)
		return false;
	return !mCurKeys[key];
}

bool Input::keyPressed(int key) const
{
	if (key < 0 || key >= MAX_KEYS)
		return false;
	if (mPrevKeys[key])
		return false;
	return mCurKeys[key];
}

bool Input::keyReleased(int key)const
{
	if (key < 0 || key >= MAX_KEYS)
		return false;
	if (mCurKeys[key])
		return false;
	return mPrevKeys[key];
}

bool Input::buttonDown(int button)const
{
	if (button < 0 || button >= MAX_BUTTONS)
		return false;
	return mCurButtons[button];
}

bool Input::buttonUp(int button)const
{
	if (button < 0 || button >= MAX_BUTTONS)
		return false;
	return !mCurButtons[button];
}

bool Input::buttonPressed(int button)const
{
	if (button < 0 || button >= MAX_BUTTONS)
		return false;
	if (mPrevButtons[button])
		return false;
	return mCurButtons[button];
}

bool Input::buttonReleased(int button)const
{
	if (button < 0 || button >= MAX_BUTTONS)
		return false;
	if (mCurButtons[button])
		return false;
	return mPrevButtons[button];
}

glm::vec2 Input::mousePosition()const
{
	return mCurMouse;
}

glm::vec2 Input::mouseDelta()const
{
	return (mPrevMouse - mCurMouse);
}

bool Input::toggleMouseLock()
{
	return ( mMouseLock = !mMouseLock );
}

bool Input::toggleMouseVisibility()
{
	setMouseVisible( !mMouseVisible );
	return mMouseVisible;
}

void Input::setMouseLock( bool enabled )
{
	mMouseLock = enabled;
}

void Input::setMouseVisible( bool visible )
{
	mMouseVisible = visible;
	SDL_ShowCursor( ( visible ? 1 : 0 ) );
}

bool Input::getMouseLock() const
{
	return mMouseLock;
}

bool Input::getMouseVisible() const
{
	return mMouseVisible;
}

bool Input::getQuit() const
{
	return mQuit;
}

Input& Input::operator=(const Input& ref)
{
	// copy values from reference
	for (int i = 0; i<MAX_KEYS; i++)
	{
		mCurKeys[i] = ref.mCurKeys[i];
		mPrevKeys[i] = ref.mPrevKeys[i];
	}

	for (int i = 0; i<MAX_BUTTONS; i++)
	{
		mCurButtons[i] = ref.mCurButtons[i];
		mPrevButtons[i] = ref.mPrevButtons[i];
	}

	mCurMouse = ref.mCurMouse;
	mPrevMouse = ref.mPrevMouse;

	mMouseLock = ref.mMouseLock;
	mMouseVisible = ref.mMouseVisible;

	return *this;
}

Input::Input(const Input& ref)
	: mCurMouse(ref.mCurMouse), mPrevMouse(ref.mPrevMouse), mMouseLock( ref.mMouseLock ), mMouseVisible( ref.mMouseVisible )
{
	// copy values from reference
	for (int i = 0; i<MAX_KEYS; i++)
	{
		mCurKeys[i] = ref.mCurKeys[i]; 
		mPrevKeys[i] = ref.mPrevKeys[i];
	}

	for (int i = 0; i<MAX_BUTTONS; i++)
	{
		mCurButtons[i] = ref.mCurButtons[i];
		mPrevButtons[i] = ref.mPrevButtons[i];
	}
}

Input::Input(SDL_Window* w)
	: mCurMouse( 0.0f ), mPrevMouse( 0.0f ), mMouseLock( true ), mMouseVisible( true )
{
	// initialize all values to false
	for (int i = 0; i<MAX_KEYS; i++)
		mCurKeys[i] = mPrevKeys[i] = false;

	for (int i = 0; i<MAX_BUTTONS; i++)
		mCurButtons[i] = mPrevButtons[i] = false;
	this->mQuit = false;
	this->mpWindow = w;
}

Input::~Input()
{
}