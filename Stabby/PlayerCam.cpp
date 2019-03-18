#include "pch.h"
#include "PlayerCam.h"
#include "ClientPlayerLC.h"

PlayerCam::PlayerCam(EntityId playerId_, int windowWidth, int windowHeight) :
	Camera{ Vec2i{20, -windowHeight / 5 + 16}, Vec2i{windowWidth, windowHeight}, 3.5 }
{
}

void PlayerCam::update(EntityId playerId) {
	ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(playerId);
	if (player != nullptr)
		center(player->getCenter() + Vec2f{0, -50});
}
