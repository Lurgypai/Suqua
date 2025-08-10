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
        int chamberSize_,
        float fireDelay_,
        float reloadDelay_,
        int bulletCount_,
        float bulletSpread_,
        float velVariance_) :
	id{ id_ },
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

void GunFireComponent::fire(Scene& currScene)
{
	auto firingPos = getFiringPos();

    for(int i = 0; i != bulletCount; ++i) {
        auto bulletId = EntitySpawnSystem::SpawnEntity(bulletTag, currScene,
                firingPos, NetworkDataComponent::Owner::local_shared);

        auto* contComp = EntitySystem::GetComp<ControllerComponent>(id);


        Vec2f directionVector{ 1.0, 0.0 };
        float baseAngle = contComp->getController().stick2.angle();
        if(bulletSpread != 0.f) {
            float angleMod = randFloat(-bulletSpread / 2.f, bulletSpread / 2.f);
            baseAngle += angleMod;
        }
        directionVector.angle(baseAngle);

        float baseVel = 260.f;
        if(velVariance != 0.f) {
            float velMod = randFloat(-velVariance / 2.f, velVariance / 2.f);
            baseVel += velMod;
        }
        auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(bulletId);
        physicsComp->setVel(directionVector * baseVel);
    }
}

void GunFireComponent::update(Scene& currScene, float delta)
{
	auto baseComp = EntitySystem::GetComp<EntityBaseComponent>(id);
	if (!baseComp->isActive) return;

    elapsedTime += delta;
    switch(state) {
    case FireState::ready: {
        auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
        // bool toggled = controllerComp->getController().toggled(ControllerBits::BUTTON_11);
        // for now we're just gonna support full auto fire and reload, more complexity to come
        bool isDown = controllerComp->getController()[ControllerBits::BUTTON_11];
        if(isDown) {
            fire(currScene);
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
        break; }
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

    /*
    std::string fireState = "none";
    switch(state) {
        case FireState::ready:
            fireState = "ready";
            break;
        case FireState::refreshing:
            fireState = "refreshing";
            break;
        case FireState::reloading:
            fireState = "reloading";
            break;
    }
    DebugIO::setLine(5, "FireState: " + fireState);
    DebugIO::setLine(6, "Shot: " + std::to_string(curShot) + " / " + std::to_string(chamberSize));
    */
}

Vec2f GunFireComponent::getFiringPos() {
	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);
	Vec2f directionVector{ 1.0, 0.0 };
	directionVector.angle(directionComp->getDir());
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	auto firingPos = posComp->pos + baseOffset + (directionVector * offset);
	return firingPos;
}
