#include "GunFireComponent.h"
#include "PhysicsComponent.h"
#include "RectDrawable.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"
#include "PositionComponent.h"
#include "NetworkOwnerComponent.h"
#include "LifeTimeComponent.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"
#include "EntityGenerator.h"

GunFireComponent::GunFireComponent(EntityId id_) :
	id{ id_ },
	offset{ 5 }
{}

EntityId GunFireComponent::getId() const {

	return id;
}

// why ref go away?
void GunFireComponent::fire(Scene* currScene)
{

	auto firingPos = getFiringPos();
	auto bullets = EntityGenerator::SpawnBasicBullet(*currScene, firingPos, NetworkOwnerComponent::Owner::local);
	EntityId bulletId = bullets[0];

	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);
	Vec2f directionVector{ 1.0, 0.0 };
	directionVector.angle(directionComp->getDir());
	auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(bulletId);
	physicsComp->setVel(directionVector * 130);

	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.0, 1.0, 0.0, 1.0}, true, -0.1, AABB{{0, 0 }, {4, 4}} });
}

void GunFireComponent::update(Scene* currScene)
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
	auto firingPos = posComp->getPos() + (directionVector * offset);
	return firingPos;
}
