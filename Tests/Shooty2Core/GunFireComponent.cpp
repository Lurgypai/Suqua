#include "GunFireComponent.h"
#include "PhysicsComponent.h"
#include "RectDrawable.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"
#include "PositionComponent.h"
#include "HitboxComponent.h"
#include "TeamComponent.h"
#include "NetworkOwnerComponent.h"
#include "LifeTimeComponent.h"
#include "DebugIO.h"
#include "DamageComponent.h"
#include "BasicDamageCalculator.h"
#include "EntityBaseComponent.h"
#include "HealthComponent.h"

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
	EntityId bulletId = currScene->addEntities(1)[0];
	EntitySystem::MakeComps<PhysicsComponent>(1, &bulletId);
	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);
	EntitySystem::MakeComps<HitboxComponent>(1, &bulletId);
	EntitySystem::MakeComps<TeamComponent>(1, &bulletId);
	EntitySystem::MakeComps<NetworkOwnerComponent>(1, &bulletId);
	EntitySystem::MakeComps<LifeTimeComponent>(1, &bulletId);
	EntitySystem::MakeComps<DamageComponent>(1, &bulletId);

	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{0.0, 1.0, 0.0, 1.0}, true, -0.1, AABB{{0, 0 }, {4, 4}} });

	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);

	Vec2f directionVector{ 1.0, 0.0 };
	directionVector.angle(directionComp->getDir());

	auto physics = EntitySystem::GetComp<PhysicsComponent>(bulletId);
	physics->setVel(directionVector * 120);
	physics->setCollideable(false);

	auto posComp = EntitySystem::GetComp<PositionComponent>(id);

	Vec2f offset{ 1.0, 0.0 };
	offset.angle(directionComp->getDir());
	physics->teleport(posComp->getPos() + (directionVector * 13));
	
	auto hitComp = EntitySystem::GetComp<HitboxComponent>(bulletId);
	hitComp->hitbox = { {0, 0}, {4, 4} };

	auto teamComponent = EntitySystem::GetComp<TeamComponent>(bulletId);
	teamComponent->teamId = 0;

	auto netOwnerComp = EntitySystem::GetComp<NetworkOwnerComponent>(bulletId);
	netOwnerComp->owner = NetworkOwnerComponent::Owner::local;

	auto lifeTimeComp = EntitySystem::GetComp<LifeTimeComponent>(bulletId);
	lifeTimeComp->setRemainingLife(480);

	auto damageComp = EntitySystem::GetComp<DamageComponent>(bulletId);
	damageComp->setDamageCalculator<BasicDamageCalculator>(10);
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