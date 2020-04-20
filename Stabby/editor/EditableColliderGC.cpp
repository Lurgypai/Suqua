
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
		rect->r = 0.0;
		rect->g = 0.0;
		rect->b = 1.0;
		break;
	case StageElement::collideable:
		rect->r = 1.0;
		rect->g = 0.0;
		rect->b = 0.0;
		break;
	case StageElement::spawnable:
		if (editable->isDefaultSpawn()) {
			rect->r = 0.0;
			rect->g = 1.0;
			rect->b = 0.0;
		}
		else {
			rect->r = 0.5;
			rect->g = 0.8;
			rect->b = 0.5;
		}
		break;
	default:
		rect->r = 0.5;
		rect->g = 0.5;
		rect->b = 0.5;
		break;
	}
}

const EntityId EditableColliderGC::getId() const {
	return id;
}
