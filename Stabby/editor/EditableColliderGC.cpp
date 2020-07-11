
#include "EditableColliderGC.h"
#include "RenderComponent.h"
#include "RectDrawable.h"
#include "EditableStageComponent.h"

EditableColliderGC::EditableColliderGC(EntityId id_) :
	id{id_}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
			EntitySystem::GetComp<RenderComponent>(id)->loadDrawable<RectDrawable>();
		}
	}
}

void EditableColliderGC::update() {
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	RectDrawable* rect = render->getDrawable<RectDrawable>();

	EditableStageComponent* editable = EntitySystem::GetComp<EditableStageComponent>(id);
	
	rect->shape = editable->getCollider();

	switch (editable->getType())
	{
	case StageElement::climbable:
		rect->c = { 0.0, 0.0, 1.0, 1.0 };
		break;
	case StageElement::collideable:
		rect->c = { 1.0, 0.0, 0.0, 1.0 };
		break;
	case StageElement::spawnable:
		if (editable->isDefaultSpawn()) {
			rect->c = { 0.0, 1.0, 0.0, 1.0 };
		}
		else {
			rect->c = { 0.5, 0.8, 0.5, 1.0 };
		}
		break;
	default:
		rect->c = { 0.5, 0.5, 0.5, 1.0 };
		break;
	}
}

const EntityId EditableColliderGC::getId() const {
	return id;
}
