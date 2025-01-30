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
            int chamberSize_,
            float fireDelay_,
            float reloadDelay_,
            int bulletCount_ = 1,
            float bulletSpread_ = 0.f,
            float velVariance_ = 0.f);

	void fire(Scene& currScene);
	void update(Scene& currScene, float delta);
	Vec2f getFiringPos();

    Vec2f baseOffset;
	float offset;
    std::string bulletTag;
    // number of shots before reload is triggered
    int chamberSize;
    // delay between shots
    float fireDelay;
    // delay to reload
    float reloadDelay;

    // bullets fired per shot
    int bulletCount;
    // spread on each shot
    float bulletSpread;
    // variance in velocity in each shot
    float velVariance;
private:
    enum class FireState : unsigned char {
        ready, //ready to fire
        refreshing, //between regular shots
        reloading // reloading
    };
    FireState state;
    // internal timer to track time since last event
    float elapsedTime;
    int curShot;
};
