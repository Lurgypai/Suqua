#include "AimToMouseComponent.h"
#include "DirectionComponent.h"
#include "ControllerComponent.h"
#include "NetworkDataComponent.h"
#include "GLRenderer.h"
#include "ParentComponent.h"
#include "PositionComponent.h"

AimToMouseComponent::AimToMouseComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<DirectionComponent>() || !EntitySystem::GetComp<DirectionComponent>(id)) {
			EntitySystem::MakeComps<DirectionComponent>(1, &id);
		}
		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}
	}
}

EntityId AimToMouseComponent::getId() const {
	return id;
}

void AimToMouseComponent::update() {
	auto controllerComp = EntitySystem::GetComp<ControllerComponent>(id);
	auto& controller = controllerComp->getController();

	//uses current camera, make sure to set the camera before updating
	Vec2f targetPos = GLRenderer::screenToWorld(controller.pointerPos);
	auto posComp = EntitySystem::GetComp<PositionComponent>(id);
	Vec2f delta = targetPos - posComp->getPos();
	
	auto directionComp = EntitySystem::GetComp<DirectionComponent>(id);
	directionComp->setDir(delta.angle());
}