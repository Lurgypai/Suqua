#pragma once
#include "ComponentMacros.h"
#include "Scene.h"

class GunFireComponent {
    CompMembers(GunFireComponent);
public:
	GunFireComponent(EntityId id_,
            float offset_,
            const std::string& bulletTag_);

	void fire(Scene* currScene);
	void update(Scene* currScene);
	Vec2f getFiringPos();

	float offset;
    std::string bulletTag;
private:
};
