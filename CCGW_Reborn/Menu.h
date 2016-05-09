#pragma once
#include <GL\glew.h>
#include <vector>
#include <string>
#include <SDL\SDL_image.h>
#include "Input.h"
#include "ForwardProgram.h"
#include "global_variables.h"
#include "GameData.h"
#include "Game.h"
struct Vertex {
	float x, y;
	float u, v;
};

struct Number {
	glm::vec2 pos;
	int number;
};

enum MENU {
	MAIN_MENU, ACTION_HUD, LOSING_SCREEN
};

class Menu {
private:
	class Button {
	public:
		Button(float startX, float startY, float width, float height, char type, GLuint texture, GLuint bufferID) {
			mTexture = texture;
			mVboID = bufferID;
			mType = type;
			mNdcX = startX; mNdcY = startY;
			mX = (startX + 1) * (gWidth / 2); mY = (startY + 1) * (gHeight / 2); mW = (startX + width + 1) * (gWidth / 2); mH = (startY + height + 1) * (gHeight / 2);
			mHighlighted = false;
		};
		~Button() {
	
		};
		bool checkMouseIntersection(int x, int y) {
			return !(x < mX || y < mY || x > mW || y > mH);
		};
		glm::vec2 getPos() const{
			return glm::vec2(mNdcX, mNdcY);
		}
		int mX, mY, mW, mH;
		float mNdcX, mNdcY;
		char mType;
		bool mHighlighted;
		GLuint mTexture;
		GLuint mVboID;
	};
public:
	struct AMenu {
		std::vector<Button> theMenu;
		std::vector<Number> theNumbers;
	};

	bool mActive;
	MENU activeMenu;
	bool update(Input* inputs, GameData* data, State state);
	void updateNumbers(GameData* data);
	void render();
	
	Menu();
	~Menu();
private:
	void buttonAction(char type, Input* inputs, int index, GameData* data);
	GLuint loadTex(std::string filePath);
	std::string readBuild(std::string filePath);
	void addNumber(float width, float height);
	void renderNumbers();
	void buildAMenu(std::string build, MENU menu);
	Button addButton(float startX, float startY, float width, float height, char type, std::string texPath);

	std::vector<AMenu> mAllMenu;
	Button* mActiveField;
	bool mRunning;
	ForwardProgram* menuShader;
	ForwardProgram* numberShader;
	GLuint numberVbo;
	GLuint numberTex;
	GLuint buttonTex;
	GLuint buttonBool;
	GLuint posLocation;
	GLuint numberLocation;
};