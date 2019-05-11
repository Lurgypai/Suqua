#include "pch.h"
#include "PlayerCam.h"
#include "ClientPlayerLC.h"
#include <iostream>

PlayerCam::PlayerCam(EntityId playerId_, int windowWidth, int windowHeight) :
	Camera{ Vec2i{0, 0}, Vec2i{windowWidth, windowHeight}, 1 }
{
}

void PlayerCam::update(EntityId playerId) {
	ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(playerId);
	if (player != nullptr) {
		center(player->getCenter());
	}
}
