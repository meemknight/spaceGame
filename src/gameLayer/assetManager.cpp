#include "assetManager.h"



void AssetManager::loadAll()
{

	auto load = [&](const char *p)
	{
		gl2d::Texture t;
		t.loadFromFile(p, false, true);
		return t;
	};

	emptyCard = load(RESOURCES_PATH "cards/empty.png");
	earthCard = load(RESOURCES_PATH "cards/earth.png");

}
