#include "stdafx.h"
#include "Stage.h"
#include "AABBLC.h"
#include "ImageGC.h"

Stage::Stage() :
	pos{ -(STAGE_WIDTH / 2), 0 }
{
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<AABBLC>(1, &id);
	EntitySystem::GetComp<AABBLC>(id)->setRes({STAGE_WIDTH, STAGE_HEIGHT});
	EntitySystem::GetComp<AABBLC>(id)->setPos(pos);
}


void Stage::loadGraphics(std::string filePath) {
	EntitySystem::MakeComps<ImageGC>(1, &id);
	ImageGC * image = EntitySystem::GetComp<ImageGC>(id);
	image->loadImageFromFile(filePath);
	image->getSprite().setPos(pos);
	image->getSprite().setScale({STAGE_WIDTH / image->getSprite().getImgRes().x, STAGE_HEIGHT / image->getSprite().getImgRes().y });
}

EntityId Stage::getId() {
	return id;
}
