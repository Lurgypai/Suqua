#pragma once
#include "GLRenderer.h"
#include "RenderComponent.h"]
#include "PositionComponent.h"

class RenderSystem {
public:
	void drawAll();
	void draw(RenderComponent& render);
};

void RenderSystem::drawAll() {
	if (EntitySystem::Contains<RenderComponent>()) {
		for (auto & comp : EntitySystem::GetPool<RenderComponent>()) {
			draw(comp);
		}
	}
}

inline void RenderSystem::draw(RenderComponent& render) {
	if (EntitySystem::Contains<RenderComponent>()) {
		IDrawable* drawable = render.sprite.get();
		if (drawable != nullptr) {
			PositionComponent* position = EntitySystem::GetComp<PositionComponent>(render.getId());
			drawable->setPos(position->pos - render.offset);
			drawable->draw();
		}
	}
}
