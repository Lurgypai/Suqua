#pragma once
#include "EntitySystem.h"
#include "Scene.h"

class GunFireComponent {
public:
	GunFireComponent(EntityId id_ = 0);
	EntityId getId() const;

	void fire(Scene* currScene);
	void update(Scene* currScene);
	Vec2f getFiringPos();

	float offset;
    std::string bulletTag;
private:
	EntityId id;
};
