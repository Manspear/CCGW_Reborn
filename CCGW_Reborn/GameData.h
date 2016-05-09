#pragma once

#include <vector>

class Assets;
class Camera;
class DeferredProgram;
class ForwardProgram;
class BillboardProgram;
class Player;
class Emission;
class Grid;
class Molebat;
class Moleratman;
class Tower;
struct GameData
{
	Assets* pAssets;
	Camera* pCamera;
	DeferredProgram* pDeferredProgram;
	ForwardProgram* pForwardProgram;
	ForwardProgram* pMenuProgram;
	BillboardProgram* pBillboardProgram;
	Player* pPlayer;
	Emission* pEmission;
	Grid* pGrid;
	Molebat* pMolebats;
	int boxScale;
	int mMolebats;
	int mBabyCount;
	Moleratman* pMoleratmen;
	int mMoleratmen;
	int pScore;
	int pGold;
	//std::vector<Tower*> mpTowers;
	Tower* pTowers;
	int mTowers;
};