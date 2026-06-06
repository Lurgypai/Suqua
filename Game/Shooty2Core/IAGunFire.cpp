#include "IAGunFire.h"
#include "PhysicsComponent.h"
#include "LifeTimeComponent.h"
#include "HealthComponent.h"
#include "EntitySpawnSystem.h"
#include "RandomUtil.h"
#include "InventoryItem.h"

IAGunFire::IAGunFire(float offset_,
        const std::string& bulletTag_,
        int chamberSize_,
        float fireDelay_,
        float reloadDelay_,
        int bulletCount_,
        float bulletSpread_,
        float velVariance_) :
	offset{ offset_  },
    bulletTag{ bulletTag_ },
    chamberSize{ chamberSize_ },
    fireDelay{ fireDelay_ },
    reloadDelay{ reloadDelay_ },
    bulletCount{bulletCount_},
    bulletSpread{bulletSpread_},
    velVariance{velVariance_}, 
    state{ FireState::ready },
    elapsedTime{ 0.f },
    curShot{ 0 }
{}

void IAGunFire::doAbility(Scene& scene, EntityId sourceEntity, const Controller& controller, InventoryItem& sourceInvItem)
{
    if (state != FireState::ready) return;

    // do firing process
	auto firingPos = getFiringPos(sourceInvItem.heldPos, sourceInvItem.angle);
    for(int i = 0; i != bulletCount; ++i) {
        auto bulletId = EntitySpawnSystem::SpawnEntity(bulletTag, scene,
                firingPos, NetworkDataComponent::Owner::local_shared);


        float baseAngle = controller.stick2.angle();
        if(bulletSpread != 0.f) {
            float angleMod = randFloat(-bulletSpread / 2.f, bulletSpread / 2.f);
            baseAngle += angleMod;
        }
        Vec2f directionVector{ 1.f, 0.f };
        directionVector.angle(baseAngle);

        float baseVel = 260.f;
        if(velVariance != 0.f) {
            float velMod = randFloat(-velVariance / 2.f, velVariance / 2.f);
            baseVel += velMod;
        }
        auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(bulletId);
        physicsComp->vel = directionVector * baseVel;
    }

    // adjust timers/delays
	++curShot;
	if(curShot < chamberSize) {
		state = FireState::refreshing;
	}
	else {
		state = FireState::reloading;
		curShot = 0;
	}
	elapsedTime = 0;
}

void IAGunFire::update(float delta)
{

    elapsedTime += delta;
    switch(state) {
    case FireState::ready:
        if (curShot != 0 && elapsedTime > reloadDelay) curShot = 0;
        break;
    case FireState::refreshing:
        if(elapsedTime > fireDelay) {
            state = FireState::ready;
            elapsedTime = 0;
        }
        break;
    case FireState::reloading:
        if(elapsedTime > reloadDelay) {
            state = FireState::ready;
            elapsedTime = 0;
        }
        break;
    }
}

std::unique_ptr<ItemAbility> IAGunFire::clone() const {
    return std::make_unique<IAGunFire>(*this);
}

Vec2f IAGunFire::getFiringPos(Vec2f basePos, float angle) {
	Vec2f directionVector{ offset, 0.0 };
    directionVector.angle(angle);
    auto firingPos = basePos + directionVector;
	return firingPos;
}
