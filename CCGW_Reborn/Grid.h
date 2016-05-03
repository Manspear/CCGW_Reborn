#pragma once

#include <vector>
#include "tempMesh.h"

#define NODEAT(x,y) ((y)*mWidth+(x))

enum
{
	TILE_EMPTY = 0,
	TILE_BOX = 0x1,
	TILE_BALLISTA = 0x2,
	TILE_HOLD = 0x4,
};

struct sNode
{
	int x, y;
	sNode* parent;
};

typedef unsigned char uchar;

//Grid class used to hold information about the gameboard.
class Grid {
public:
	//Make sure path is large enough to hold all the targets. A good estimate would be atleast half the size of the map.
	bool findPath( sNode start, sNode end, sNode* path, int* targets );

	//Determines if a tile has a/several specific flag/flags.
	bool tileIs( int x, int y, uchar flags ) const;

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

	void debugRender( GLuint programID );

	Grid( int width, int height, tempMesh* debugMesh);
	Grid();
	~Grid();

private:
	int heuristic( sNode* start, sNode* end );
	int mScale;
	int mWidth, mHeight;
	uchar* mpGrid;
	int *mGScore, *mFScore;
	sNode* mPath;
	sNode* mPath2;

	tempMesh* DEBUG_mesh;
};