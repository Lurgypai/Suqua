#include "pch.h"
#include "PlayerGC.h"
#include "ClientPlayerLC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "PhysicsAABB.h"
#include "ImageGC.h"
#include "HeadParticleLC.h"



PlayerGC::PlayerGC(EntityId id_) :
	id{ id_ },
	sprite{ "images/none.png", {16, 16 } },
	offset{0, 0},
	justDied{false}
{}

void PlayerGC::load(std::string filePath) {
	try {
		sprite = AnimatedSprite{
			filePath,
			Vec2i{64, 64}

		};
	}
	catch (FileNotFoundException e) {
		std::cout << e.what() << '\n';
	}

	//keep in mind graphics gale starts at frame 1, not 0
	sprite.addAnimation(idle, 0, 1);
	sprite.addAnimation(walking, 1, 17);
	sprite.addAnimation(slash1, 17, 20);
	sprite.addAnimation(slash2, 20, 24);
	sprite.addAnimation(slash3, 24, 30);
	sprite.addAnimation(roll, 30, 38);
	sprite.addAnimation(stun, 38, 39);
	sprite.addAnimation(dead, 39, 51);

	//ClientPlayerLC* playerLogic = EntitySystem::GetComp<ClientPlayerLC>(id);
	//Vec2f res = playerLogic->getRes();
	Vec2f res = { 4.0f, 20.0f };
	offset = Vec2f{ (sprite.getObjRes().abs().x - res.x) / 2, (sprite.getObjRes().abs().y - res.y) };
}

EntityId PlayerGC::getId() const {
	return id;
}

unsigned int PlayerGC::getRenderBuffer() {
	return sprite.getRenderBufferId();
}

ImgData PlayerGC::getImgData() const {
	return sprite.getImgData();
}

void PlayerGC::spawnHead(Vec2f pos, std::string filePath) {
	unsigned int id_;
	EntitySystem::GenEntities(1, &id_);
	EntitySystem::MakeComps<HeadParticleLC>(1, &id_);
	EntitySystem::MakeComps<ImageGC>(1, &id_);

	HeadParticleLC * particle = EntitySystem::GetComp<HeadParticleLC>(id_);
	particle->setLife(240);
	particle->setPos(pos);
	particle->setVel({ 0, -80 });

	ImageGC * image = EntitySystem::GetComp<ImageGC>(id_);
	image->loadImageFromFile(filePath);
}
