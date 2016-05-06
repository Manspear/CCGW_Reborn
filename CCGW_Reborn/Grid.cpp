#include "Grid.h"
#include <iostream>

bool Grid::findPath( sNode start, sNode end, sNode* path, int* targets )
{
	bool result = false;

	std::vector<sNode*> openList;
	std::vector<sNode*> closedList;

	// Initialize paths to default values.
	// All scores should be above any reasonable value to ensure that the
	// default value is never shorter than a found path.
	for( int i=0; i<mWidth*mHeight; i++ )
	{
		mGScore[i] = 999999;
		mFScore[i] = 999999;
		mPath[i].x = i%mWidth;
		mPath[i].y = i/mWidth;
		mPath[i].parent = nullptr;
	}
	
	// Set scores for the inital node
	mGScore[NODEAT(start.x,start.y)] = 0;
	mFScore[NODEAT(start.x,start.y)] = heuristic( &start, &end );
	
	// Add inital node to the open list
	sNode* first = &mPath[NODEAT(start.x,start.y)];
	openList.push_back(first);

	// Keep looping until the open list is empty and there are no more candidates for movement
	while( openList.size() > 0 && !result )
	{
		// Find the node in the open list that has the shortest approximated distance to the target
		std::vector<sNode*>::iterator currentIT = openList.end();
		int currentScore = 999999;
		for( std::vector<sNode*>::iterator it = openList.begin(); it != openList.end(); it++ )
		{
			int score = mFScore[NODEAT((*it)->x,(*it)->y)];
			if( score < currentScore )
			{
				currentScore = score;
				currentIT = it;
			}
		}

		// If no node was found, something has gone horribly wrong
		if( currentIT == openList.end() )
			throw -1;

		// If the best candidate is the goal, we have found a path
		if( (*currentIT)->x == end.x && (*currentIT)->y == end.y )
		{
			// Go through each nodes parent to compile a path
			sNode* parent = *currentIT;
			int cur = 0;
			while( parent != nullptr )
			{
				path[cur++] = *parent;
				parent = parent->parent;
			}

			*targets = cur;
			result = true;
		}
		else
		{
			// Move candidate from open list to closed list, showing that we have visited it
			closedList.push_back(*currentIT);
			openList.erase( currentIT );
			currentIT = closedList.end()-1;

			// Make sure we don't go out of bounds
			int minx = (*currentIT)->x-1;
			int miny = (*currentIT)->y-1;
			int maxx = minx+2;
			int maxy = miny+2;
			int curx = (*currentIT)->x;
			int cury = (*currentIT)->y;

			if( minx < 0 )
				minx = 0;
			if( miny < 0 )
				miny = 0;
			if( maxx > mWidth-1 )
				maxx = mWidth-1;
			if( maxy > mHeight-1 )
				maxy = mHeight-1;

			// Loop over the 3x3 square centered on the current node
			for( int x=minx; x <=maxx; x++ )
			{
				for( int y=miny; y<=maxy; y++ )
				{
					// If this is the current node, we don't need to process it
					if( x == curx && y == cury )
						continue;

					// Make sure that we're not accessing diagonal nodes
					// and make sure the tile is empty
					if( ( x == curx || y == cury ) && mpGrid[NODEAT(x,y)] == TILE_EMPTY )
					{
						// Make sure the node has not already been visited
						sNode* dir = &mPath[NODEAT(x,y)];
						bool found = false;
						for( std::vector<sNode*>::iterator it = closedList.begin(); it != closedList.end() && !found; it++ )
						{
							if( (*it)->x == dir->x &&(*it)->y == dir->y )
								found = true;
						}

						// Make sure the node is not already a candidate for visit
						for (std::vector<sNode*>::iterator it = openList.begin(); it != openList.end() && !found; it++)
						{
							if ((*it)->x == dir->x && (*it)->y == dir->y)
								found = true;
						}

						if( !found )
						{
							// The cost for traversing this node is the cost of all the previous nodes + 1
							int tscore = mGScore[NODEAT(curx,cury)] + 1;

							// Only consider node if it's score is lower than the current node
							if( tscore < mGScore[NODEAT(x,y)] )
							{
								// Add node to open list
								dir->parent = *currentIT;
								openList.push_back( dir );

								// Set score of node
								mGScore[NODEAT(x,y)] = tscore;
								mFScore[NODEAT(x,y)] = tscore + heuristic( dir, &end );
							}
						}
					}
				}
			}
		}
	}

	return result;
}

bool Grid::tileIs( int x, int y, uchar flags ) const
{
	return ( ( getTile( x, y ) & flags ) > 0 );
}

void Grid::setTile( int x, int y, uchar flags )
{
	if( x >= 0 && x < mWidth && y >= 0 && y < mHeight )
		mpGrid[y*mWidth+x] = flags;
}

uchar Grid::getTile( int x, int y ) const
{
	if( x >= 0 && x < mWidth && y >= 0 && y < mHeight )
		return mpGrid[y*mWidth+x];
	return TILE_OUT_OF_BOUNDS;
}

uchar* Grid::getGrid() const
{
	return mpGrid;
}

int Grid::getWidth() const
{
	return mWidth;
}

int Grid::getHeight() const
{
	return mHeight;
}

sNode* Grid::getPath() 
{
	return mPath2;
}

int Grid::heuristic( sNode* start, sNode* end )
{
	int difx = end->x - start->x;
	int dify = end->y - start->y;
	return ( difx*difx + dify*dify );
}

/*void Grid::debugRender( GLuint programID )
{
	for( int z=0, i=0; z<mHeight; z++ )
	{
		for( int x=0; x<mWidth; x++, i++ )
		{
			uchar tile = mpGrid[i];
			if( tile == TILE_BOX )
			{
				GLuint worldLocation = glGetUniformLocation( programID, "world" );
				glm::mat4 world;
				world *= glm::vec4(2, 2, 2, 1);
				world[3][0] = x;
				world[3][1] = 0.5f;
				world[3][2] = z;

				glUniformMatrix4fv( worldLocation, 1, GL_FALSE, &world[0][0] );

				DEBUG_mesh->draw();
			}
		}
	}
}*/

Grid::Grid( int width, int height)
	: mWidth( width ), mHeight( height )
{
	mpGrid = new uchar[width*height];
	mGScore = new int[width*height];
	mFScore = new int[width*height];
	mPath = new sNode[width*height];
	mPath2 = new sNode[width*height];
	for( int i=0; i<width*height; i++ )
	{
		mpGrid[i] = TILE_EMPTY;
	}
}

Grid::Grid()
{

}

Grid::~Grid()
{
	delete[] mpGrid;
	delete[] mGScore;
	delete[] mFScore;
	delete[] mPath;
	delete[] mPath2;
}