#include "ButtonComponent.h"
#include "ControllerComponent.h";
#include "RenderComponent.h"
#include "NetworkDataComponent.h"
#include "PositionData.h"
#include "AABB.h"

ButtonComponent::ButtonComponent(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<ControllerComponent>() || !EntitySystem::GetComp<ControllerComponent>(id)) {
			EntitySystem::MakeComps<ControllerComponent>(1, &id);
		}
	}
}

EntityId ButtonComponent::getId() const {
	return id;
}

void ButtonComponent::update() {
	ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(id);
	auto& controller = cont->getController();

	if (controller.toggled(ControllerBits::BUTTON_11) && controller[ControllerBits::BUTTON_11]) {
		auto* drawable = EntitySystem::GetComp<RenderComponent>(id)->getDrawable<Sprite>();
		auto* data = EntitySystem::GetComp<NetworkDataComponent>(id);
		AABB bounding = { { data->get<float>(X),
							data->get<float>(Y) },
						  { drawable->getObjRes().x * drawable->getScale().x,
							drawable->getObjRes().y * drawable->getScale().y } };

		if (bounding.contains(controller.pointerPos)) {
			state = !state;
		}
	}
}
