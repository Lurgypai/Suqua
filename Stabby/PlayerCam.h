#pragma once
#include "GLRenderer.h"
#include "EntitySystem.h"

struct PlayerCam : public Camera {
public:
	PlayerCam(EntityId playerId_, int windowWidth, int windowHieght);
	void update(EntityId playerId);
};