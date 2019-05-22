#include "stdafx.h"
#include "Stage.h"
#include "AABB.h"
#include "ImageGC.h"

Stage::Stage() :
	pos{ -(STAGE_WIDTH / 2), 0 },
	collider{ { -(STAGE_WIDTH / 2), 0 }, Vec2f{STAGE_WIDTH, STAGE_HEIGHT} }
{
	EntitySystem::GenEntities(1, &id);
}

const AABB& Stage::getCollider() const {
	return collider;
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
