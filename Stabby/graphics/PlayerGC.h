#pragma once
#include "GLRenderer.h"
#include "EntitySystem.h"
#include "AnimatedSprite.h"
#include "RenderComponent.h"
#include "IDrawable.h"

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
	void loadAnimations();
	//void spawnHead(Vec2f pos);

	void updateState(double timeDelta);

	EntityId getId() const;
	AnimatedSprite animSprite;
	AnimatedSprite attackSprite;
private:
	EntityId id;

	bool shouldSpawnHead;
	State prevState;
	float prevXVel;
	int prevAttack;
	double defaultFrameDelay;
};