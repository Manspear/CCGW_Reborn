#pragma once

#include <vector>
#include <glm\glm.hpp>
#include "Frustum.h"

#define NODEAT(x,y) ((y)*mWidth+(x))
#define GRID_MAX_HIERARCHIES 3

enum
{
	TILE_EMPTY = 0,
	TILE_BOX = 0x1,
	TILE_BALLISTA = 0x2,
	TILE_HOLD = 0x4,
	TILE_BLOCKED = 0x8,
	TILE_OUT_OF_BOUNDS = 0b11111111,
};

struct sNode
{
	int x, y;
	sNode* parent;
};

struct sBoxHier
{
	BoundingBox mBoundingBox;
	sBoxHier* mpChildren;
};

typedef unsigned char uchar;

class Tower;
//Grid class used to hold information about the gameboard.
class Grid {
public:
	//Make sure path is large enough to hold all the targets. A good estimate would be atleast half the size of the map.
	bool findPath( sNode start, sNode end, sNode* path, int* targets );

	//Determines if a tile has a/several specific flag/flags.
	bool tileIs( int x, int y, uchar flags ) const;

	void cull( const Frustum* frustum, Tower* towers, Tower** visible, int* max );

	//Set a tile to a/several specific flag/flags.
	void setTile( int x, int y, uchar flags );
	//Returns the flags of a tile.
	uchar getTile( int x, int y ) const;
	//Returns the entire grid.
	uchar* getGrid() const;
	//Returns the width of the grid.
	int getWidth() const;
	//Returns the height of the grid.
	int getHeight() const;
	sNode* getPath();

	//Grid( int width, int height, tempMesh* mesh);
	Grid( int width, int height );
	Grid();
	~Grid();

	sBoxHier mCullHierarchy[GRID_MAX_HIERARCHIES];

private:
	int heuristic( sNode* start, sNode* end );
	void buildCullHierarchy( sBoxHier* parent, float scale );
	void destroyCullHierarchy( sBoxHier* parent );

	int mScale;
	int mWidth, mHeight;
	uchar* mpGrid;
	int *mGScore, *mFScore;
	sNode* mPath;
	sNode* mPath2;

	std::vector<sNode*> mOpenList;
	std::vector<sNode*> mClosedList;
};