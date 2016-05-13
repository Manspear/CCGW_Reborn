#include "Menu.h"
#include <fstream>

bool Menu::update(Input * inputs, GameData* data, State state)
{
	if (inputs->getQuit())
		return false;
	if (inputs->keyPressed(SDLK_ESCAPE))
	{
		mActive = true;
		activeMenu = MAIN_MENU;
		inputs->setMouseVisible(true);
		inputs->setMouseLock(false);
	}
	if (state == GAME_LOST) {
		activeMenu = LOSING_SCREEN;
		mActive = true;
		inputs->setMouseVisible(true);
		inputs->setMouseLock(false);
	}
	if (mActive == ACTION_HUD) {
		
	}
	else {
		int x = inputs->mousePosition().x;
		int y = (inputs->mousePosition().y * -1) + gHeight;
		for (int i = 0; i < mAllMenu[activeMenu].theMenu.size(); i++) {
			if (mAllMenu[activeMenu].theMenu[i].checkMouseIntersection(x, y)) {
				if (mAllMenu[activeMenu].theMenu[i].mType != 'd')
					mAllMenu[activeMenu].theMenu[i].mHighlighted = true;
				if (inputs->buttonReleased(0)) {
					buttonAction(mAllMenu[activeMenu].theMenu[i].mType, inputs, i, data);
					break;
				}
			}
			else
				mAllMenu[activeMenu].theMenu[i].mHighlighted = false;
		}
	}
	if (mActiveField != nullptr) {
			writeToField(inputs->getPressedKeys());			
	}
	render();
	if (activeMenu != MAIN_MENU)
		updateNumbers(data);
	return mRunning;
}

void Menu::writeToField(std::vector<int>* keyVector)
{
	int x;
	for (int i = 0; i < keyVector->size(); i++) {
		x = keyVector->at(i);
		if (x == 8) {
			if (!mAllMenu[activeMenu].theNumbers.empty())
				mAllMenu[activeMenu].theNumbers.pop_back();
		}
		else if (mAllMenu[activeMenu].theNumbers.size() < 6 ){
			if (x > 57)
				x -= 86;
			else
				x -= 48;
			mAllMenu[activeMenu].theNumbers.push_back(Number{ mActiveField->getPos() + glm::vec2(mAllMenu[activeMenu].theNumbers.size() * 0.05f, 0), x });
		}
	}
}

void Menu::writeToHighScore()
{
	ofstream file;
	file.open("models/menu/highscore.txt");
	file << 875;
	file.close();
}

void Menu::buttonAction(char type, Input* inputs, int index, GameData* data)
{
	switch (type) {
	case'p':
		activeMenu = ACTION_HUD;
		mActive = false;
		mActiveField = nullptr;
		inputs->setMouseLock(true);
		inputs->setMouseVisible(false);
		break;
	case'q':
		mRunning = false;
		mActiveField = nullptr;
		writeToHighScore();
		break;
	case'r':
		activeMenu = ACTION_HUD;
		mActive = false;
		inputs->setMouseLock(true);
		inputs->setMouseVisible(false);
		mActiveField = nullptr;
		data->pGame->restartGame();
		break;
	case'd':
		mAllMenu[activeMenu].theMenu[index].mHighlighted = false;
		mActiveField = nullptr;
		break;
	case 'a':
		mActiveField = &mAllMenu[activeMenu].theMenu[index];
		break;
	}
}

void Menu::updateNumbers(GameData * data)
{
	if( activeMenu == LOSING_SCREEN )
		return;
}

void Menu::render()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	menuShader->use();	
	for (int i = mAllMenu[activeMenu].theMenu.size() - 1; i > -1; i--) {
		glUniform1i(buttonTex, 0);
		glActiveTexture(GL_TEXTURE0);

		glUniform1i(buttonBool, mAllMenu[activeMenu].theMenu[i].mHighlighted);

		glBindTexture(GL_TEXTURE_2D, mAllMenu[activeMenu].theMenu[i].mTexture);
		glBindBuffer(GL_ARRAY_BUFFER, mAllMenu[activeMenu].theMenu[i].mVboID);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	menuShader->unUse();
	renderNumbers();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Menu::renderNumbers()
{
	numberShader->use();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	GLuint texLocation = glGetUniformLocation(numberShader->getProgramID(), "texSampler");
	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, numberTex);
	for (int i = 0; i < mAllMenu[activeMenu].theNumbers.size(); i++) {
		glUniform2fv(posLocation, 1, &mAllMenu[activeMenu].theNumbers[i].pos[0]);
		glUniform1f(numberLocation, mAllMenu[activeMenu].theNumbers[i].sampleX);
		glBindBuffer(GL_ARRAY_BUFFER, numberVbo);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	numberShader->unUse();
}

void Menu::buildAMenu(std::string build, MENU menu)
{
	istringstream s;
	s.str(readBuild(build));
	float x, y, w, h;
	char a, b, c, d, e;
	string temp, texPath;
	char type, nrOrButt;
	istringstream s2;
	mAllMenu.push_back(AMenu{});
	activeMenu = menu;
	while (getline(s, temp)) {
		s2.str(temp);	
		s2 >> nrOrButt;
		if (nrOrButt == 'b') {
			s2 >> x >> y >> w >> h >> type >> texPath;
			mAllMenu[menu].theMenu.push_back(addButton(x, y, w, h, type, texPath));
		}
		else if (nrOrButt == 'n') {
			s2 >> x >> y;
			mAllMenu[menu].theNumbers.push_back(Number{ glm::vec2(x, y), 0 });
		}
		else {
			mActiveField = &mAllMenu[menu].theMenu.back();
			s2 >> x;
			std::vector<int> aVector;
			while (!s2.eof()) {
				s2 >> a;
				aVector.push_back(a);
			}
			writeToField(&aVector);
		}
		s2.clear();
	}
}

Menu::Button Menu::addButton(float startX, float startY, float width, float height, char type, std::string texPath)
{
	GLuint vboID;
	glGenBuffers(1, &vboID);

	Vertex vertexData[6];

	//first triangle
	vertexData[0].x = startX + width;
	vertexData[0].y = startY + height;
	vertexData[0].u = 1;
	vertexData[0].v = 0;

	vertexData[1].x = startX;
	vertexData[1].y = startY + height;
	vertexData[1].u = 0;
	vertexData[1].v = 0;

	vertexData[2].x = startX;
	vertexData[2].y = startY;
	vertexData[2].u = 0;
	vertexData[2].v = 1;

	//second triangle
	vertexData[3].x = startX;
	vertexData[3].y = startY;
	vertexData[3].u = 0;
	vertexData[3].v = 1;

	vertexData[4].x = startX + width;
	vertexData[4].y = startY;
	vertexData[4].u = 1;
	vertexData[4].v = 1;

	vertexData[5].x = startX + width;
	vertexData[5].y = startY + height;
	vertexData[5].u = 1;
	vertexData[5].v = 0;

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return Button(startX, startY, width, height, type, loadTex(texPath), vboID);
}

void Menu::addNumber(float width, float height)
{
	numberTex = loadTex("Models/menu/numbers.png");
	glGenBuffers(1, &numberVbo);
	
	Vertex vertexData[6];

	vertexData[0].x = width;
	vertexData[0].y = height;
	vertexData[0].u = 0.1f;
	vertexData[0].v = 1;

	vertexData[1].x = 0;
	vertexData[1].y = height;
	vertexData[1].u = 0;
	vertexData[1].v = 1;

	vertexData[2].x = 0;
	vertexData[2].y = 0;
	vertexData[2].u = 0;
	vertexData[2].v = 0;

	//second triangle
	vertexData[3].x = 0;
	vertexData[3].y = 0;
	vertexData[3].u = 0;
	vertexData[3].v = 0;

	vertexData[4].x = width;
	vertexData[4].y = 0;
	vertexData[4].u = 0.1f;
	vertexData[4].v = 0;

	vertexData[5].x = width;
	vertexData[5].y = height;
	vertexData[5].u = 0.1f;
	vertexData[5].v = 1;

	glGenBuffers(1, &numberVbo);
	glBindBuffer(GL_ARRAY_BUFFER, numberVbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Menu::Menu()
{
	menuShader = new ForwardProgram("menu.vertex", "menu.pixel", " ");
	numberShader = new ForwardProgram("numbers.vertex", "numbers.pixel", " ");
	menuShader->setClear(false);
	numberShader->setClear(false);
	mActiveField = nullptr;
	mActive = true;
	mRunning = true;
	posLocation = glGetUniformLocation(numberShader->getProgramID(), "position");;
	numberLocation = glGetUniformLocation(numberShader->getProgramID(), "number");;
	buttonTex = glGetUniformLocation(menuShader->getProgramID(), "texSampler");
	buttonBool = glGetUniformLocation(menuShader->getProgramID(), "highlighted");

	buildAMenu("menuBuild.txt", MAIN_MENU);
	buildAMenu("actionBuild.txt", ACTION_HUD);
	buildAMenu("losingBuild.txt", LOSING_SCREEN);
	activeMenu = MAIN_MENU;

	addNumber(0.05, 0.07);
}

Menu::~Menu()
{
	delete menuShader;
	delete numberShader;
	glDeleteBuffers(1, &numberVbo);
}

GLuint Menu::loadTex(std::string filePath)
{
	SDL_Surface* img = IMG_Load(filePath.c_str());
	GLuint texID = 0;
	if (img)
	{
		GLenum format = GL_RGBA;
		if (img->format->BitsPerPixel == 24)
			format = GL_RGB;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, format, GL_UNSIGNED_BYTE, img->pixels);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		SDL_FreeSurface(img);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	return texID;
}

std::string Menu::readBuild(std::string filePath)
{
	std::ifstream shaderFile("Models/menu/" + filePath);
	if (shaderFile.fail()) {
		std::cout << "Fil fel: " + filePath;
	}
	std::stringstream fileContent;
	std::string line;
	while (std::getline(shaderFile, line)) {
		fileContent << line << "\n";
	}
	shaderFile.close();
	return fileContent.str();
}
