#pragma once
#include "ComponentMacros.h"
#include "Scene.h"

class GunFireComponent {
    CompMembers(GunFireComponent);
public:
	GunFireComponent(EntityId id_,
            const Vec2f& baseOffset_,
            float offset_,
            const std::string& bulletTag_,
            int bulletCount_ = 1,
            float bulletSpread_ = 0.f,
            float velVariance_ = 0.f);

	void fire(Scene& currScene);
	void update(Scene& currScene);
	Vec2f getFiringPos();

    Vec2f baseOffset;
	float offset;
    std::string bulletTag;

    int bulletCount;
    float bulletSpread;
    float velVariance;
private:
};
