#pragma once
#include "ComponentMacros.h"
#include "Scene.h"

class GunFireComponent {
    CompMembers(GunFireComponent);
public:
	GunFireComponent(EntityId id_,
            const Vec2f& baseOffset_,
            float offset_,
            const std::string& bulletTag_);

	void fire(Scene& currScene);
	void update(Scene& currScene);
	Vec2f getFiringPos();

    Vec2f baseOffset;
	float offset;
    std::string bulletTag;
private:
};
