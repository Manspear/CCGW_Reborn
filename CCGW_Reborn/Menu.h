#pragma once
#include <GL\glew.h>
#include <vector>
#include <string>
#include <SDL\SDL_image.h>
#include "Assets.h"
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
	int sampleX;
	GLuint vboID;
	GLuint texID;
};

struct Score {
	string name;
	int wave, babies, gold;
};

enum MENU {
	MAIN_MENU, ACTION_HUD, TACTICAL_HUD, LOSING_SCREEN, VICTORY_SCREEN, PAUSE_SCREEN, TUTORIAL_SCREEN, HIGHSCORE_SCREEN, WRITE_HIGHSCORE
};

class Menu {
private:
	class Button {
	public:
		Button(float startX, float startY, float width, float height, int type, GLuint texture, GLuint bufferID) {
			mTexture = texture;
			mVboID = bufferID;
			mType = type;
			mNdcX = startX; mNdcY = startY;
			mX = (startX + 1) * (gWidth / 2); mY = (startY + 1) * (gHeight / 2); mW = (startX + width + 1) * (gWidth / 2); mH = (startY + height + 1) * (gHeight / 2);
			mHighlighted = false;
			move = glm::vec2(0);
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
		int mType;
		bool mHighlighted;
		GLuint mTexture;
		GLuint mVboID;
		glm::vec2 move;
	};
public:
	struct AMenu {
		std::vector<Button> theMenu;
		std::vector<Number> theNumbers;
		GLuint texID;
	};

	bool mActive;
	MENU activeMenu;
	bool update(Input* inputs, GameData* data, State state);
	void updateNumbers(GameData* data);
	void render();
	
	Menu();
	~Menu();
private:
	void buttonAction(int type, Input* inputs, GameData* data);
	GLuint loadTex(std::string filePath);
	std::string readBuild(std::string filePath);
	void addNumber(float width, float height, GLuint &vboID, GLuint &texID, string filePath);
	void renderNumbers();
	void buildAMenu(std::string build, MENU menu, GLuint vboID, GLuint texID);
	void healthBar();
	Button addButton(float startX, float startY, float width, float height, char type, std::string texPath);
	void setPauseState(MENU theMenu, Input* input);
	void writeToField(std::vector<int>* keyVector);
	void writeToFieldString(std::string theString);
	void pausedMenu(Input* inputs);
	void readFromHighScore();
	void moveMarker(float power);
	bool checkIfHighscore(GameData* data);
	void writeHighscore(GameData* data, int index);
	string getScoreHolder();

	string highscoreHolder, mWaveScore, mBabyScore, mGoldScore;
	int bestWave, bestBaby, bestGold;
	int cursorHolder;
	std::vector<AMenu> mAllMenu;
	Button* mActiveField;
	bool mRunning;
	ForwardProgram* menuShader;
	ForwardProgram* numberShader;
	GLuint numberVbo;
	GLuint numberTex;
	GLuint healthbarVbo;
	GLuint healthbarTex;
	GLuint buttonTex;
	GLuint buttonBool;
	GLuint posLocation;
	GLuint numberLocation;
	GLuint moveLocation;
	Assets mAssets;
	Score highScores[10];
	MENU lastHUD;
};