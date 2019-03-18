#pragma once
#include "GLRenderer.h"
#include "EntitySystem.h"
#include "AnimatedSprite.h"
#include "PlayerLC.h"

enum Animation {
	idle,
	walking,
	slash1,
	slash2,
	slash3,
	roll,
	stun,
	dead
};

class PlayerGC {
public:
	PlayerGC(EntityId id_ = 0);
	void load(std::string filePath);
	EntityId getId() const;
	unsigned int getRenderBuffer();
	ImgData getImgData() const;
	void spawnHead(Vec2f pos, std::string filePath);

	template<typename T>
	void update();
private:
	Vec2f offset;
	AnimatedSprite sprite;
	EntityId id;
	Animation anim;
	bool justDied;
};


template<typename T>
void PlayerGC::update() {
	T * player = EntitySystem::GetComp<T>(id);
	if (player != nullptr) {


		int width = sprite.getObjRes().abs().x;
		int height = sprite.getObjRes().abs().y;
		sprite.setObjRes(Vec2i{ player->getFacing() * width, height });

		sprite.setPos(player->getPos() - offset);
		
		State plrState = player->getState();
		switch (plrState) {
		case State::dead:
			if (!justDied) {
				spawnHead(player->getPos(), player->getHeadPath());
				justDied = true;
			}
			sprite.setAnimation(dead);
			if (sprite.getFrame() != sprite.getAnimation(dead).y - 1)
				sprite.forward();
			break;
		case State::stunned:
			sprite.setAnimation(stun);
			sprite.forward();
			break;
		case State::rolling:
			sprite.setAnimation(roll);
			sprite.forward();
			break;
		case State::attacking:
			if (player->getActiveId() == 1) {
				sprite.setAnimation(slash1);
				if (sprite.getFrame() != sprite.getAnimation(slash1).y - 1)
					sprite.forward();
			}
			else if (player->getActiveId() == 2) {
				sprite.setAnimation(slash2);
				if (sprite.getFrame() != sprite.getAnimation(slash2).y - 1)
					sprite.forward();
			}
			else if (player->getActiveId() == 3) {
				sprite.setAnimation(slash3);
				if (sprite.getFrame() != sprite.getAnimation(slash3).y - 1)
					sprite.forward();
			}
			break;
		case State::free:
			justDied = false;
			if (player->getVel().x == 0) {
				sprite.setAnimation(idle);
			}
			else {
				sprite.setAnimation(walking);
			}
			sprite.forward();
			break;
		}
	}
}
