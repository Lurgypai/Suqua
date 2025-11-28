#pragma once
#include "ItemAbility.h"

class IAGunFire : public ItemAbility {
public:
	IAGunFire(const Vec2f& baseOffset_,
            float offset_,
            const std::string& bulletTag_,
            int chamberSize_,
            float fireDelay_,
            float reloadDelay_,
            int bulletCount_ = 1,
            float bulletSpread_ = 0.f,
            float velVariance_ = 0.f);

    virtual void doAbility(Scene& scene, EntityId sourceEntity, Vec2f stick1, Vec2f stick2, InventoryItem& sourceInvItem);
    virtual void update(float delta) override;
    virtual std::unique_ptr<ItemAbility> clone() const override;

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
	Vec2f getFiringPos(EntityId id);

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
