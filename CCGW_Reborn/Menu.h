#pragma once
#include <GL\glew.h>
#include <vector>
#include <string>
#include <SDL\SDL_image.h>
#include "Input.h"
#include "ForwardProgram.h"
#include "global_variables.h"

struct Vertex {
	float x, y;
	float u, v;
};

class Menu {
public:
	bool mActive;
	char update(Input* inputs);
	void render();
	void addButton(float startX, float startY, float width, float height, char type, std::string texPath);
	Menu(std::string filePath);
	Menu();
	~Menu();
private:
	class Button {
	public:
		Button(float startX, float startY, float width, float height, char type, GLuint texture, GLuint bufferID) {
			mTexture = texture;
			mVboID = bufferID;
			mType = type;
			mX = (startX + 1) * (gWidth / 2); mY = (startY + 1) * (gHeight / 2); mW = (startX + width + 1) * (gWidth / 2); mH = (startY + height + 1) * (gHeight / 2);
		};
		~Button() {

		};
		bool checkMouseIntersection(int x, int y) {
			return !(x < mX || y < mY || x > mW || y > mH);
		};
		int mX, mY, mW, mH;
		char mType;
		GLuint mTexture;
		GLuint mVboID;
	};

	GLuint loadTex(std::string filePath);
	std::string readBuild(std::string filePath);
	std::vector<Button> mButtons;
	ForwardProgram* menuShader;
};