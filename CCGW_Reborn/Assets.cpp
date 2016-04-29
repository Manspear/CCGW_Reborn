#include "Assets.h"

void Assets::unload()
{
	for (map<string, Asset*>::iterator it = mAssets.begin(); it != mAssets.end(); it++)
	{
		it->second->unload();
		delete it->second;
	}

	mAssets.clear();
}

Assets& Assets::operator=(const Assets& ref)
{
	mAssets = ref.mAssets;
	return *this;
}

Assets::Assets(const Assets& ref)
	: mAssets(ref.mAssets)
{
}

Assets::Assets()
{
}

Assets::~Assets()
{
	unload();
}