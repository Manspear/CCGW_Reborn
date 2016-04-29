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
	BillboardProgram* pBillboardProgram;
	Player* pPlayer;
	Emission* pEmission;
	Grid* pGrid;
	Molebat* pMolebats;
	int boxScale;
	int mMolebats;
	Moleratman* pMoleratmen;
	int mMoleratmen;
	std::vector<Tower*> mpTowers;
};