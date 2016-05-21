#include "Menu.h"
#include "Player.h"
#include <fstream>

void Menu::moveMarker(float power)
{
	mAllMenu[ACTION_HUD].theMenu[0].move = glm::vec2(0, 0.055 * power);
}

bool Menu::update(Input * inputs, GameData* data, State state)
{
	if (inputs->getQuit())
		return false;
	if (mActive) {
		int x = inputs->mousePosition().x;
		int y = (inputs->mousePosition().y * -1) + gHeight;
		for (int i = 0; i < mAllMenu[activeMenu].theMenu.size(); i++) {
			if (mAllMenu[activeMenu].theMenu[i].checkMouseIntersection(x, y)) {
				if (mAllMenu[activeMenu].theMenu[i].mType != 0)
					mAllMenu[activeMenu].theMenu[i].mHighlighted = true;
				if (inputs->buttonReleased(0) && mAllMenu[activeMenu].theMenu[i].mType != 0) {
					buttonAction(mAllMenu[activeMenu].theMenu[i].mType, inputs, data);
					break;
				}
			}
			else
				mAllMenu[activeMenu].theMenu[i].mHighlighted = false;
		}
	}
	else {
		if (activeMenu == ACTION_HUD && data->pGame->tactical && data->pWavespawner->getWave() > 0)
			data->pWavespawner->playSound(25);
		activeMenu = (MENU)(1 + data->pGame->tactical);
		updateNumbers(data);
		moveMarker(data->pPlayer->getStrength());
		if (state == GAME_LOST)
			setPauseState(LOSING_SCREEN, inputs);
		else if (state == GAME_WON) {
			setPauseState(VICTORY_SCREEN, inputs);	
		}
	}
	if (inputs->keyPressed(SDLK_ESCAPE)) {
		if (mActive)
			buttonAction(lastHUD, inputs, data);
		else {
			lastHUD = activeMenu;
			buttonAction(5, inputs, data);
		}
	}
	if (mActiveField != nullptr) {
		writeToField(inputs->getPressedKeys());
	}
	render();
	return mRunning;
}

void Menu::buttonAction(int type, Input* inputs, GameData* data)
{
	pausedMenu(inputs);
	activeMenu = (MENU)type;
	switch (type) {
	case 0:
		activeMenu = lastHUD;
		break;
	case 1:
		inputs->setMouseVisible(false);
		inputs->setMouseLock(!data->pGame->tactical);
		mActive = false;
		break;
	case 2:
		inputs->setMouseVisible(false);
		inputs->setMouseLock(data->pGame->tactical);
		mActive = false;
	case 10:
		mRunning = false;
		activeMenu = (MENU)0;
		break;
	case 11:
		data->pGame->restartGame();
		healthBar();
		buttonAction(1, inputs, data);
		break;
	case 5:
		mActive = true;
		break;
	case 7:
		readFromHighScore();
		break;
	case 8:
		mActiveField = &mAllMenu[activeMenu].theMenu[0];
		mAllMenu[activeMenu].theMenu[0].mHighlighted = true;
		break;
	case 15:
		if (checkIfHighscore(data)) {
			data->pGame->restartGame();
			healthBar();
			buttonAction(7, inputs, data);
			mActiveField = nullptr;
			break;
		}
		else {
			data->pGame->restartGame();
			healthBar();
			buttonAction(0, inputs, data);
			mActiveField = nullptr;
			break;
		}
	case 16:
		activeMenu = MAIN_MENU;
		break;
	}
}

void Menu::updateNumbers(GameData * data)
{
	int divider = 1;
	for (int i = 0; i < 2; i++) {
		mAllMenu[activeMenu].theNumbers[i].sampleX = (data->mBabyCount / divider) % 10;
		divider *= 10;
	}
	divider = 1;
	for (int i = 4; i < 7; i++) {
		mAllMenu[activeMenu].theNumbers[i].sampleX = (data->pGold / divider) % 10;
		divider *= 10;
	}
	divider = 1;
	for (int i = 2; i < 4; i++) {
		mAllMenu[activeMenu].theNumbers[i].sampleX = (data->pWavespawner->getWave()/ divider) % 10;
		divider *= 10;
	}
	
	if (activeMenu == ACTION_HUD) {
		int hp = data->pPlayer->getHealth() / 10;
		while (hp < mAllMenu[ACTION_HUD].theNumbers.size() - 7)
			mAllMenu[ACTION_HUD].theNumbers.pop_back();
	}
}

void Menu::setPauseState(MENU theMenu, Input* inputs)
{
	mActive = true;
	activeMenu = theMenu;
	inputs->setMouseVisible(true);
	inputs->setMouseLock(false);
}

void Menu::pausedMenu(Input* inputs) 
{
	mActive = true;
	inputs->setMouseVisible(true);
	inputs->setMouseLock(false);
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
		glUniform2fv(moveLocation, 1, &mAllMenu[activeMenu].theMenu[i].move[0]);
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
	for (int i = 0; i < mAllMenu[activeMenu].theNumbers.size(); i++) {
		glBindTexture(GL_TEXTURE_2D, mAllMenu[activeMenu].theNumbers[i].texID);
		glUniform2fv(posLocation, 1, &mAllMenu[activeMenu].theNumbers[i].pos[0]);
		glUniform1f(numberLocation, mAllMenu[activeMenu].theNumbers[i].sampleX);
		glBindBuffer(GL_ARRAY_BUFFER, mAllMenu[activeMenu].theNumbers[i].vboID);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	numberShader->unUse();
}

void Menu::buildAMenu(std::string build, MENU menu, GLuint vboID, GLuint texID)
{
	istringstream s;
	s.str(readBuild(build));
	float x, y, w, h;
	char a, b, c, d, e;
	string temp, texPath;
	int type;
	char nrOrButt;
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
			mAllMenu[menu].theNumbers.push_back(Number{ glm::vec2(x, y), 0, vboID, texID});
		}
		nrOrButt = ' ';
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

void Menu::addNumber(float width, float height, GLuint &vboID, GLuint &texID, string filePath)
{
	texID = loadTex(filePath);
	glGenBuffers(1, &vboID);
	
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

	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Menu::healthBar()
{
	for(int i = 0; i < 10; i++)
		mAllMenu[ACTION_HUD].theNumbers.push_back(Number{ glm::vec2(-0.439 + i*0.088, -0.8264), 0, healthbarVbo, healthbarTex });
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
	lastHUD = MAIN_MENU;
	cursorHolder = 0;
	addNumber(0.04, 0.06, numberVbo, numberTex, "Models/menu/numbers.png");
	addNumber(0.088, 0.0612, healthbarVbo, healthbarTex, "Models/menu/healthbar.png");
	posLocation = glGetUniformLocation(numberShader->getProgramID(), "position");;
	numberLocation = glGetUniformLocation(numberShader->getProgramID(), "number");;
	buttonTex = glGetUniformLocation(menuShader->getProgramID(), "texSampler");
	buttonBool = glGetUniformLocation(menuShader->getProgramID(), "highlighted");
	moveLocation = glGetUniformLocation(menuShader->getProgramID(), "movement");
	buildAMenu("menuBuild.txt", MAIN_MENU, numberVbo, numberVbo);
	buildAMenu("actionBuild.txt", ACTION_HUD, numberVbo, numberTex);
	buildAMenu("tacticalBuild.txt", TACTICAL_HUD, numberVbo, numberTex);
	buildAMenu("losingBuild.txt", LOSING_SCREEN, numberVbo, numberTex);
	buildAMenu("victoryBuild.txt", VICTORY_SCREEN, numberVbo, numberTex);
	buildAMenu("pauseBuild.txt", PAUSE_SCREEN, numberVbo, numberTex);
	buildAMenu("tutorialBuild.txt", TUTORIAL_SCREEN, numberVbo, numberTex);
	buildAMenu("highscoreBuild.txt", HIGHSCORE_SCREEN, numberVbo, numberTex);
	buildAMenu("writescoreBuild.txt", WRITE_HIGHSCORE, numberVbo, numberTex);
	readFromHighScore();
	healthBar();
	activeMenu = MAIN_MENU;
}

Menu::~Menu()
{
	delete menuShader;
	delete numberShader;
	glDeleteBuffers(1, &numberVbo);
	mAssets.unload();
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

void Menu::readFromHighScore()
{
	ifstream file;
	istringstream ss;
	string aString;
	int x, y, z;
	activeMenu = HIGHSCORE_SCREEN;
	file.open("models/menu/highscore.txt");
	mAllMenu[activeMenu].theNumbers.clear();
	int i = 0;
	int j = 0;
	for (int j = 0; j < 10; j++){
		getline(file, aString);
		ss.str(aString);
		ss >> highscoreHolder >> x >> y >> z;
		highScores[j] = { highscoreHolder, x, y, z };
		mWaveScore = std::to_string(x); mBabyScore = std::to_string(y); mGoldScore = to_string(z);
		mActiveField = &mAllMenu[activeMenu].theMenu[i++];
		writeToFieldString(highscoreHolder);
		mActiveField = &mAllMenu[activeMenu].theMenu[i++];
		writeToFieldString(mWaveScore);
		mActiveField = &mAllMenu[activeMenu].theMenu[i++];
		writeToFieldString(mBabyScore);
		mActiveField = &mAllMenu[activeMenu].theMenu[i++];
		writeToFieldString(mGoldScore);
		ss.clear();
	}
	file.close();
}

char getMeTheChar(int theInt) {
	return theInt + 87;
}

string Menu::getScoreHolder() {
	string name = "";
	for (int i = 0; i < mAllMenu[WRITE_HIGHSCORE].theNumbers.size(); i++) {
		name += getMeTheChar(mAllMenu[WRITE_HIGHSCORE].theNumbers[i].sampleX);
	}
	mAllMenu[WRITE_HIGHSCORE].theNumbers.clear();
	return name;
}

bool Menu::checkIfHighscore(GameData* data) {
	highscoreHolder = getScoreHolder();
	bestWave = data->pWavespawner->getWave(); 
	bestBaby = data->mBabyCount;
	bestGold = data->pGold;
	for (int i = 0; i < 10; i++){
		if (bestWave > highScores[i].wave) {
			writeHighscore(data, i);
			return true;
		}
		if (bestBaby > highScores[i].babies) {
			writeHighscore(data, i);
			return true;
		}
		if (bestGold > highScores[i].gold) {
			writeHighscore(data, i);
			return true;
		}
	}
	return false;
}

void Menu::writeHighscore(GameData* data, int index)
{
	ofstream file;
	file.open("models/menu/highscore.txt");
	int j = 0;
	for (int i = 0; i < 10; i++) {
		if (i != index) {
			file << highScores[j].name << " " << highScores[j].wave << " " << highScores[j].babies << " " << highScores[j].gold;
			j++;
		}
		else {
			file << highscoreHolder << " " << bestWave << " " << bestBaby << " " << bestGold;
		}
		if (i != 9)
			file << "\n";
	}	
	file.close();
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
			else if (mAllMenu[WRITE_HIGHSCORE].theNumbers.size() < 5) {
				if (x > 57)
					x -= 87;
				else
					x -= 48;
				mAllMenu[activeMenu].theNumbers.push_back(Number{ mActiveField->getPos() + glm::vec2(mAllMenu[activeMenu].theNumbers.size() * 0.04f, 0), x, numberVbo, numberTex });
			}
	}
}

void Menu::writeToFieldString(std::string theString)
{
	int x;
	for (int i = 0; i < theString.size(); i++) {
		x = theString[i];
		if (x > 57)
			x -= 87;
		else
			x -= 48;
		mAllMenu[activeMenu].theNumbers.push_back(Number{ mActiveField->getPos() + glm::vec2(cursorHolder * 0.04f, 0), x, numberVbo, numberTex });
		cursorHolder++;
	}
	cursorHolder = 0;
	mActiveField = nullptr;
}

