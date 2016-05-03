#include "Menu.h"
#include <fstream>

char Menu::update(Input * inputs)
{
	char result = 0;
	if (inputs->buttonReleased(0)) {		
		int x = inputs->mousePosition().x;
		int y = (inputs->mousePosition().y * -1) + gHeight;
		for (int i = 0; i < mButtons.size(); i++) {
			if (mButtons[i].checkMouseIntersection(x, y)) {
				result = mButtons[i].mType;
				break;
			};
		}
	}
	return result;
}

void Menu::render()
{
	menuShader->use();
	for (int i = 0; i < mButtons.size(); i++) {
		GLuint texLocation = glGetUniformLocation(menuShader->getProgramID(), "texSampler");
		glUniform1i(texLocation, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mButtons[i].mTexture);

		glBindBuffer(GL_ARRAY_BUFFER, mButtons[i].mVboID);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	menuShader->unUse();
}

void Menu::addButton(float startX, float startY, float width, float height, char type, std::string texPath)
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(Vertex, x));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)offsetof(Vertex, u));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	mButtons.push_back(Button(startX, startY, width, height, type, loadTex(texPath), vboID));
}

Menu::Menu(std::string filePath)
{
	menuShader = new ForwardProgram("menu.vertex", "menu.pixel", " ");
	menuShader->setClear(false);
	mActive = true;
	
	istringstream s;
	s.str(readBuild(filePath));
	float x, y, w, h;
	string temp, texPath;
	char type;
	istringstream s2;
	while (getline(s, temp)) {		
		s2.str(temp);
		s2 >> x >> y >> w >> h >> type >> texPath;
		addButton(x, y, w, h, type, texPath);
		s2.clear();
	}
}

Menu::Menu()
{
	menuShader = new ForwardProgram("menu.vertex", "menu.pixel", " ");
	menuShader->setClear(false);
	mActive = true;
}

Menu::~Menu()
{
	delete menuShader;
}

GLuint Menu::loadTex(std::string filePath)
{
	SDL_Surface* img = IMG_Load(filePath.c_str());
	GLuint texID = 0;
	if (img)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img->w, img->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
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
