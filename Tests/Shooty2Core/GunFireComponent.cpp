#include "GunFireComponent.h"
#include "PhysicsComponent.h"
#include "RectDrawable.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"
#include "PositionComponent.h"

GunFireComponent::GunFireComponent(EntityId id_) :
	id{ id_ },
	offset{ 5 }
{}

EntityId GunFireComponent::getId() const {

	return id;
}

void GunFireComponent::fire(Scene& currScene)
{
	EntityId bulletId = currScene.addEntities(1)[0];
	EntitySystem::MakeComps<PhysicsComponent>(1, &bulletId);
	EntitySystem::MakeComps<RenderComponent>(1, &bulletId);

	auto render = EntitySystem::GetComp<RenderComponent>(bulletId);
	render->loadDrawable<RectDrawable>(RectDrawable{ Color{1.0, 1.0, 0.0, 1.0}, true, 0.0, AABB{{0, 0 }, {4, 4}} });

	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);

	Vec2f directionVector{ 1.0, 0.0 };
	directionVector.angle(directionComp->getDir());

	auto physics = EntitySystem::GetComp<PhysicsComponent>(bulletId);
	physics->setVel(directionVector * 20);

	auto posComp = EntitySystem::GetComp<PositionComponent>(id);

	Vec2f offset{ 1.0, 0.0 };
	offset.angle(directionComp->getDir());
	physics->teleport(posComp->getPos() + (directionVector * 5));
}

void GunFireComponent::update(Scene& currScene)
{
	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	if (controllerComp->getController()[ControllerBits::BUTTON_1]) {
		fire(currScene);
	}
}