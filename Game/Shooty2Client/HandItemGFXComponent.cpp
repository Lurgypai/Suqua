#include "HandItemGFXComponent.h"
#include "PositionComponent.h"
#include "RenderComponent.h"
#include "Sprite.h"
#include "../Shooty2Core/HandComponent.h"

HandItemGFXComponent::HandItemGFXComponent(EntityId id_) :
	id{id_},
	items{}
{
    if (!EntitySystem::Contains<RenderComponent>() || !EntitySystem::GetComp<RenderComponent>(id)) {
        EntitySystem::MakeComps<RenderComponent>(1, &id);
    }

    auto renderComp = EntitySystem::GetComp<RenderComponent>(id);
	items.reserve(HandComponent::SLOT_COUNT);
	for (int i = 0; i != HandComponent::SLOT_COUNT; ++i) {
		size_t spriteIndex = renderComp->allocateDrawable();
		items.push_back(RenderItem{spriteIndex, "" });
	}
}

inline static void updateItem(int index,
	const std::vector<RenderItem> items,
	PositionComponent* posComp,
	HandComponent* invComp,
	RenderComponent* renderComp) {

	auto& item = items[index];
	if (!renderComp->hasDrawable(item.spriteIndex)) return;

	Sprite& sprite = renderComp->getDrawable<Sprite>(item.spriteIndex);
	sprite.offset = invComp->getHandPos(index) - posComp->pos;

	float dir = invComp->getHandAngle(index);
	int dirDeg = dir * 180 / 3.14159;
	if (dirDeg < -90 || dirDeg > 90) {
		sprite.verticalFlip = true;
	}
	else {
		sprite.verticalFlip = false;
	}

	if (dirDeg >= -135 && dirDeg < -45) {
		sprite.setDepth(1.0);
	}
	else {
		sprite.setDepth(-1.0);
	}

	sprite.setAngle(dir * 180 / 3.14159);
}

void HandItemGFXComponent::update(const HandItemGFXSystem& invItemGfxSys) {
	auto* posComp = EntitySystem::GetComp<PositionComponent>(id);
	auto* invComp = EntitySystem::GetComp<HandComponent>(id);
	auto* renderComp = EntitySystem::GetComp<RenderComponent>(id);

	for (int i = 0; i != HandComponent::SLOT_COUNT; ++i) {
		if (!invComp->handIsActive(i)) continue;
		auto& item = items[i];
		// item has changed update sprite
		if (invComp->getHandTag(i) != item.renderTag) {
			item.renderTag = invComp->getHandTag(i);
			auto invItemGfx = invItemGfxSys.getGFX(item.renderTag);
			if (invItemGfx.renderMode == HandItemGFX::sprite) {
				renderComp->setDrawable<Sprite>(item.spriteIndex, item.renderTag);
				auto& sprite = renderComp->getDrawable<Sprite>(item.spriteIndex);
				sprite.setOrigin(invItemGfx.renderOffset);
			}
			else {
				renderComp->clearDrawable(item.spriteIndex);
			}
		}
		// update if drawn to screen
		auto invItemGfx = invItemGfxSys.getGFX(item.renderTag);
		if (invItemGfx.renderMode != HandItemGFX::sprite) continue;
		updateItem(i, items, posComp, invComp, renderComp);
	}
}
