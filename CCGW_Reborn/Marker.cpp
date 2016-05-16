#include "Marker.h"
#include "Player.h"
#include "Tower.h"

void Marker::update(const Player* player) {
	float aRot = player->getRot() +glm::pi<float>() * 0.5f;

	glm::vec3 playerPos = player->getPosition();
	glm::vec3 lookAt = player->getLookAt();
	this->mWorld = {
		cosf(aRot), 0.0f, sinf(aRot), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinf(aRot), 0.0f, cosf(aRot), 0.0f,
		playerPos.x + lookAt.x * 8,			playerPos.y + lookAt.y * 8,	playerPos.z + lookAt.z * 8,	1.0
	};
}

bool Marker::update(const Input * inputs, GameData &gameData)
{
	selectedTile = mousePicking(inputs->mousePosition(), gameData) * (float)gameData.boxScale;

	int gridWidth = gameData.pGrid->getWidth();
	int gridHeight = gameData.pGrid->getHeight();

	if( selectedTile.x < 0 )
		selectedTile.x = 0;
	else if( selectedTile.x >= gridWidth * gameData.boxScale )
		selectedTile.x = gridWidth * gameData.boxScale;
	if( selectedTile.y < 0 )
		selectedTile.y = 0;
	else if( selectedTile.y >= gridHeight * gameData.boxScale )
		selectedTile.y = gridHeight * gameData.boxScale;

	uchar currentTile = gameData.pGrid->getTile(selectedTile.x / gameData.boxScale, selectedTile.y / gameData.boxScale);
	bool buildTowers = false;
	if (inputs->buttonDown(0) && gameData.pGold > 0)
	{
		if( currentTile == TILE_EMPTY )
		{
			gameData.pGrid->setTile(selectedTile.x / gameData.boxScale , selectedTile.y / gameData.boxScale, TILE_HOLD);
			mMarkedIndex.push_back(selectedTile);
			sNode start = { 0, 0 };
			sNode end = { 8, 47 };
			int mTargets = 0;
			if (!gameData.pGrid->findPath(start, end, gameData.pGrid->getPath(), &mTargets)) {
				mMarkedIndex.erase(mMarkedIndex.end() - 1);
				gameData.pGrid->setTile(selectedTile.x / gameData.boxScale, selectedTile.y / gameData.boxScale, TILE_EMPTY);
			}
			else
				gameData.pGold--;
		}
		else if( currentTile == TILE_BOX )
		{
			gameData.pGrid->setTile( selectedTile.x / gameData.boxScale, selectedTile.y / gameData.boxScale, TILE_BOX | TILE_HOLD );
			mMarkedIndex.push_back( selectedTile );
			gameData.pGold--;
		}
	}
	if (inputs->buttonDown(2) && currentTile == TILE_HOLD)
	{
		gameData.pGrid->setTile(selectedTile.x / gameData.boxScale, selectedTile.y / gameData.boxScale, TILE_EMPTY);
		for (int i = 0; i < mMarkedIndex.size(); i++) {
			if (mMarkedIndex[i] == selectedTile) {
				mMarkedIndex.erase(mMarkedIndex.begin() + i);
				gameData.pGold++;
			}
		}	
	}
	if (inputs->keyDown(SDLK_1))
	{
		buildTowers = true;
		for (int i = 0; i < mMarkedIndex.size(); i++) {
			//gameData.pGrid->setTile(mMarkedIndex[i].x / gameData.boxScale, mMarkedIndex[i].y / gameData.boxScale, TILE_BOX);

			int x = mMarkedIndex[i].x / gameData.boxScale;
			int y = mMarkedIndex[i].y / gameData.boxScale;
			int gridWidth = gameData.pGrid->getWidth();

			uchar tile = gameData.pGrid->getTile( x, y );
			if( tile == TILE_HOLD )
			{
				gameData.pGrid->setTile( x, y, TILE_BOX );
				gameData.pTowers[y*gridWidth+x].setAlive( true );
				//gameData.pTowers[y*gridWidth+x].setHasBallista( true );

				glm::vec3 ppos = gameData.pPlayer->getPosition();
				if (!gameData.pPlayer->checkMove(ppos))
				{
					gameData.pPlayer->setPosition({ ppos.x, 5, ppos.z });
				}
			}
			else if( tile == TILE_BOX | TILE_HOLD )
			{
				gameData.pGrid->setTile( x, y, TILE_BALLISTA );
				gameData.pTowers[y*gridWidth+x].setHasBallista( true );
			}
		}

		mMarkedIndex.clear();
	}
	mWorld[3][0] = selectedTile.x;
	mWorld[3][1] = 1.5f;
	mWorld[3][2] = selectedTile.y;
	return buildTowers;
}

//void Marker::render(const GLuint & programID)
void Marker::render( GLuint worldLocation )
{
	for( int i=0; i<3; i++ )
		mWorld[i][i] = 1.0f;

	//GLuint world = glGetUniformLocation(programID, "world");
	glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &mWorld[0][0]);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mpModel->drawNonAni();
	for (int i = 0; i < mMarkedIndex.size(); i++) {
		mWorld[3][0] = mMarkedIndex[i].x;
		mWorld[3][1] = 1.0f;
		mWorld[3][2] = mMarkedIndex[i].y;
		glUniformMatrix4fv(worldLocation, 1, GL_FALSE, &mWorld[0][0]);
		mpModel->drawNonAni();
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

std::vector<glm::vec2> Marker::getMarkedTiles()
{
	return mMarkedIndex;
}

void Marker::resetMarkedTiles()
{
	mMarkedIndex.clear();
}

glm::vec2 Marker::mousePicking(const glm::vec2 mousePos, const GameData &gameData) {
	float x = (2.0f * mousePos.x + 40) / gWidth - 1.0f;
	float y = 1.0f - (2.0f * mousePos.y + 60) / gHeight;
	glm::vec3 rayNDC = glm::vec3(x, y, 0);
	glm::vec4 rayClipSpace = glm::vec4(rayNDC.x, rayNDC.y, -1.0, 0.0);
	glm::vec4 rayEyeSpace = glm::inverse(gameData.pCamera->getPerspective()) * rayClipSpace;
	rayEyeSpace = glm::vec4(rayEyeSpace.x, rayEyeSpace.y, -1.0, 0.0);
	glm::vec4 rayWorldSpace = glm::inverse(gameData.pCamera->getView()) * rayEyeSpace;
	rayWorldSpace = glm::normalize(rayWorldSpace);
	float scalar = -(gameData.pCamera->getPosition().y - 0.5) / rayWorldSpace.y;
	rayWorldSpace *= scalar;
	glm::vec2 pickPos;
	pickPos.x = (int)(gameData.pCamera->getPosition().x / gameData.boxScale + rayWorldSpace.x);
	pickPos.y = (int)(gameData.pCamera->getPosition().z / gameData.boxScale + rayWorldSpace.z);
	//std::cout << pickPos.x << "    " << pickPos.y << "\n";
	return pickPos;
}

Marker::Marker()
{
	mPicked = false;
	selectedTile = { -1, -1 };
	setScale(1.f/50.f);
}

Marker::~Marker()
{

}
