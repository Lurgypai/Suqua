#pragma once
#include "GLRenderer.h"
#include "EntitySystem.h"
#include "AnimatedSprite.h"
#include "RenderComponent.h"
#include "IDrawable.h"

#include "../combat/WeaponManager.h"
#include "../combat/AttackTag.h"

#include "player.h"

class PlayerGC {
public:
	enum Animation {
		idle,
		walking,
		roll,
		stun,
		dead,
		climb,
		heal,
		crouch
	};

	PlayerGC(EntityId id_ = 0);
	void loadAnimations(const WeaponManager& weapons);
	void loadNameTag();
	//void spawnHead(Vec2f pos);

	void updateState(double timeDelta);

	EntityId getId() const;
	EntityId getNameTageRenderId() const;
	std::unordered_map<std::string, AnimatedSprite> attackAnimations;
	std::string currAttackTag;
	AnimatedSprite animSprite;
private:
	EntityId id;
	//rendercomponent for the nametag
	EntityId nameTagId;

	bool shouldSpawnHead;
	State prevState;
	float prevXVel;
	int prevAttack;
	double defaultFrameDelay;
	bool animationFrozen;
};