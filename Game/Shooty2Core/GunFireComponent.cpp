#include "GunFireComponent.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"
#include "PositionComponent.h"
#include "LifeTimeComponent.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "EntitySpawnSystem.h"
#include "RandomUtil.h"

GunFireComponent::GunFireComponent(EntityId id_,
        const Vec2f& baseOffset_,
        float offset_,
        const std::string& bulletTag_,
        int bulletCount_,
        float bulletSpread_,
        float velVariance_) :
	id{ id_ },
    baseOffset{ baseOffset_},
	offset{ offset_  },
    bulletTag{ bulletTag_ },
    bulletCount{bulletCount_},
    bulletSpread{bulletSpread_},
    velVariance{velVariance_}
{}

void GunFireComponent::fire(Scene& currScene)
{
	auto firingPos = getFiringPos();

    for(int i = 0; i != bulletCount; ++i) {
        auto bulletId = EntitySpawnSystem::SpawnEntity(bulletTag, currScene,
                firingPos, NetworkDataComponent::Owner::local_shared);

        auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);


        Vec2f directionVector{ 1.0, 0.0 };
        if(bulletSpread != 0.f) {
            float angleMod = randFloat(-bulletSpread / 2.f, bulletSpread / 2.f);
            directionVector.angle(contComp->getController().stick2.angle() + angleMod);
        }

        float baseVel = 260.f;
        if(velVariance != 0.f) {
            float velMod = randFloat(-velVariance / 2.f, velVariance / 2.f);
            baseVel += velMod;
        }
        auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(bulletId);
        physicsComp->setVel(directionVector * baseVel);
    }
}

void GunFireComponent::update(Scene& currScene)
{
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	if (!baseComp->isActive) return;

	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (controllerComp->getController().toggled(ControllerBits::BUTTON_11)) {
		if (controllerComp->getController()[ControllerBits::BUTTON_11]) {
			fire(currScene);
		}
	}
}

Vec2f GunFireComponent::getFiringPos() {
	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);
	Vec2f directionVector{ 1.0, 0.0 };
	directionVector.angle(directionComp->getDir());
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	auto firingPos = posComp->getPos() + baseOffset + (directionVector * offset);
	return firingPos;
}
