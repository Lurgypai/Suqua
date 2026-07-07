#include "IAGunFire.h"
#include "PhysicsComponent.h"
#include "HealthComponent.h"
#include "TeamComponent.h"
#include "EntitySpawnSystem.h"
#include "RandomUtil.h"

IAGunFire::IAGunFire(float offset_,
        const std::string& bulletTag_,
        int chamberSize_,
        float fireDelay_,
        float reloadDelay_,
        int bulletCount_,
        float bulletSpread_,
        float baseVel_,
        float velVariance_) :
	offset{ offset_  },
    bulletTag{ bulletTag_ },
    chamberSize{ chamberSize_ },
    fireDelay{ fireDelay_ },
    reloadDelay{ reloadDelay_ },
    bulletCount{bulletCount_},
    bulletSpread{bulletSpread_},
    baseVel{baseVel_},
    velVariance{velVariance_}, 
    state{ FireState::ready },
    elapsedTime{ 0.f },
    curShot{ 0 }
{}

void IAGunFire::doAbility(Scene& scene, EntityId sourceEntity, EntityId targetEntity,
        const Controller& controller, const Vec2f& heldPos, float angle)
{
    if (state != FireState::ready) return;

    // do firing process
	auto firingPos = getFiringPos(heldPos, angle);
    for(int i = 0; i != bulletCount; ++i) {
        auto bulletId = EntitySpawnSystem::SpawnEntity(bulletTag, scene,
                firingPos, NetworkDataComponent::Owner::local_shared);
        auto* ownerTeam = EntitySystem::GetComp<TeamComponent>(sourceEntity);
        if(ownerTeam != nullptr) {
            auto* bulletTeam = EntitySystem::GetComp<TeamComponent>(bulletId);
            bulletTeam->teamId = ownerTeam->teamId;
        }

        float baseAngle = controller.stick2.angle();
        if(bulletSpread != 0.f) {
            float angleMod = randFloat(-bulletSpread / 2.f, bulletSpread / 2.f);
            baseAngle += angleMod;
        }
        Vec2f directionVector{ 1.f, 0.f };
        directionVector.angle(baseAngle);

        float vel = baseVel;
        if(velVariance != 0.f) {
            float velMod = randFloat(-velVariance / 2.f, velVariance / 2.f);
            vel += velMod;
        }
        auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(bulletId);
        physicsComp->vel = directionVector * vel;
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
