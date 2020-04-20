#pragma once
#include "GLRenderer.h"
#include "EntitySystem.h"

struct PlayerCam : public Camera {
public:
	PlayerCam(int windowWidth, int windowHieght);
	void update(EntityId playerId);
};