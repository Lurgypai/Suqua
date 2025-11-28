#include "IAGunFire.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"
#include "PositionComponent.h"
#include "LifeTimeComponent.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "EntitySpawnSystem.h"
#include "RandomUtil.h"

IAGunFire::IAGunFire(const Vec2f& baseOffset_,
        float offset_,
        const std::string& bulletTag_,
        int chamberSize_,
        float fireDelay_,
        float reloadDelay_,
        int bulletCount_,
        float bulletSpread_,
        float velVariance_) :
    baseOffset{ baseOffset_},
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

void IAGunFire::doAbility(Scene& scene, EntityId sourceEntity, Vec2f stick1, Vec2f stick2, InventoryItem& sourceInvItem)
{
    if (state != FireState::ready) return;

    // do firing process
	auto firingPos = getFiringPos(sourceEntity);
    for(int i = 0; i != bulletCount; ++i) {
        auto bulletId = EntitySpawnSystem::SpawnEntity(bulletTag, scene,
                firingPos, NetworkDataComponent::Owner::local_shared);


        float baseAngle = stick2.angle();
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

Vec2f IAGunFire::getFiringPos(EntityId id) {
	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);
	Vec2f directionVector{ 1.0, 0.0 };
	directionVector.angle(directionComp->getDir());
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	auto firingPos = posComp->pos + baseOffset + (directionVector * offset);
	return firingPos;
}
