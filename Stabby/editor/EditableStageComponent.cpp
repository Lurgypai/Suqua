
#include "EditableStageComponent.h"
#include "GLRenderer.h"
#include "SDL.h"
#include "EntityBaseComponent.h"
#include "PositionComponent.h"

#include <iostream>

EditableStageComponent::EditableStageComponent(EntityId id_) :
	id{id_},
	anchorPoint{0, 0},
	type{StageElement::collideable},
	collider{ {0, 0}, {0, 0} },
	anchorSide{0},
	state{EditableState::idle},
	defaultSpawn{true}
{
	if (id != 0) {
		if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PositionComponent>(1, &id);
		}
	}
}

const EntityId EditableStageComponent::getId() const {
	return id;
}

void EditableStageComponent::update(int camId) {
	Vec2i mousePos_;
	Uint32 mouseState = SDL_GetMouseState(&mousePos_.x, &mousePos_.y);

	Vec2f mousePos = GLRenderer::screenToWorld(mousePos_, camId);

	bool currButton1 = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool currButton2 = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);
	bool currButton3 = mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE);

	switch (state)
	{
	case EditableState::moving: {
		if (currButton1) {
			Vec2f delta = mousePos - Vec2f{ anchorPoint };
			collider.pos = prevPoint + delta;
		}
		else {
			state = EditableState::idle;
		}
	}
		break;
	case EditableState::resizing: {
		if (currButton1 || currButton2) {
			switch (anchorSide)
			{
			case AnchorSide::right:
				adjustRight(mousePos);
				break;
			case AnchorSide::left:
				adjustLeft(mousePos);
				break;
			case AnchorSide::top:
				adjustTop(mousePos);
				break;
			case AnchorSide::bottom:
				adjustBottom(mousePos);
				break;
			case AnchorSide::topright:
				adjustTop(mousePos);
				adjustRight(mousePos);
				break;
			case AnchorSide::bottomright:
				adjustBottom(mousePos);
				adjustRight(mousePos);
				break;
			case AnchorSide::topleft:
				adjustTop(mousePos);
				adjustLeft(mousePos);
				break;
			case AnchorSide::bottomleft:
				adjustBottom(mousePos);
				adjustLeft(mousePos);
				break;
			default:
				break;
			}
		}
		else {
			state = EditableState::idle;
			if (collider.res.x < 0) {
				collider.pos.x += collider.res.x;
				collider.res.x = -collider.res.x;
			}
			if (collider.res.y < 0) {
				collider.pos.y += collider.res.y;
				collider.res.y = -collider.res.y;
			}
		}
	}
		break;
	case EditableState::idle: {
		int outlineWidth{ 4 };
		AABB inside{ {collider.pos.x + (outlineWidth / 2), collider.pos.y + (outlineWidth / 2)}, {collider.res.x - outlineWidth, collider.res.y - outlineWidth} };
		AABB outside{ {collider.pos.x - (outlineWidth / 2), collider.pos.y - (outlineWidth / 2)}, {collider.res.x + outlineWidth, collider.res.y + outlineWidth} };

		if (currButton1 != prevButton1 && currButton1) {
			if (!inside.contains(mousePos) && outside.contains(mousePos)) {
				//grabbed outline
				anchorPoint = mousePos;
				state = EditableState::resizing;

				//find anchor side / corner
				if (mousePos.x < inside.pos.x) {
					if (mousePos.y < inside.pos.y) {
						//top right
						anchorSide = AnchorSide::topleft;
					}
					else if (mousePos.y > inside.pos.y + inside.res.y) {
						//bottom right
						anchorSide = AnchorSide::bottomleft;
					}
					else {
						//right
						anchorSide = AnchorSide::left;
					}
					prevPoint = collider.pos;
					prevRes = collider.res;
				}
				else if (mousePos.x > inside.pos.x + inside.res.x) {
					if (mousePos.y < inside.pos.y) {
						//top left
						anchorSide = AnchorSide::topright;
					}
					else if (mousePos.y > inside.pos.y + inside.res.y) {
						//bottom left
						anchorSide = AnchorSide::bottomright;
					}
					else {
						//left
						anchorSide = AnchorSide::right;
					}
					prevPoint = collider.pos;
					prevRes = collider.res;
				}
				else if (mousePos.y < inside.pos.y) {
					//top
					anchorSide = AnchorSide::top;
					prevPoint = collider.pos;
					prevRes = collider.res;
				}
				else if (mousePos.y > inside.pos.y + inside.res.y) {
					//bottom
					anchorSide = AnchorSide::bottom;
					prevPoint = collider.pos;
					prevRes = collider.res;
				}
			}
		}
		if (inside.contains(mousePos)) {
			if (currButton1 != prevButton1 && currButton1) {
				state = EditableState::moving;
				anchorPoint = mousePos;
				prevPoint = collider.pos;
			}
			if (currButton2 != prevButton2 && currButton2) {
				//toggle type
				if (type == StageElement::collideable) {
					type = StageElement::climbable;
				}
				else if (type == StageElement::climbable) {
					type = StageElement::spawnable;
					defaultSpawn = true;
				}
				else if (type == StageElement::spawnable && defaultSpawn) {
					defaultSpawn = false;
				} else if(type== StageElement::spawnable && !defaultSpawn) {
					type = StageElement::collideable;
				
				}
			}
			else if (currButton3 != prevButton3 && currButton3) {
				//delete
				EntitySystem::GetComp<EntityBaseComponent>(id)->isDead = true;
			}
		}

		if (collider.res.x == 0 || collider.res.y == 0)
			EntitySystem::GetComp<EntityBaseComponent>(id)->isDead = true;
	}
		break;
	default:
		break;
	}

	prevButton1 = currButton1;
	prevButton2 = currButton2;
	prevButton3 = currButton3;

	collider.pos = Vec2f{std::round(collider.pos.x), std::roundf(collider.pos.y) };
	collider.res = Vec2f{ std::roundf(collider.res.x), std::roundf(collider.res.y) };

	EntitySystem::GetComp<PositionComponent>(id)->pos = collider.pos;
}

bool EditableStageComponent::isDefaultSpawn() {
	return defaultSpawn;
}

const AABB& EditableStageComponent::getCollider() const {
	return collider;
}

StageElement EditableStageComponent::getType() const {
	return type;
}

void EditableStageComponent::adjustLeft(const Vec2f mousePos) {
	float delta = anchorPoint.x - mousePos.x;
	collider.pos.x = prevPoint.x - delta;
	collider.res.x = prevRes.x + delta;
}

void EditableStageComponent::adjustRight(const Vec2f mousePos) {
	float delta = mousePos.x - anchorPoint.x;
	collider.res.x = prevRes.x + delta;
}

void EditableStageComponent::adjustTop(const Vec2f mousePos) {
	float delta = anchorPoint.y - mousePos.y;
	collider.pos.y = prevPoint.y - delta;
	collider.res.y = prevRes.y + delta;
}

void EditableStageComponent::adjustBottom(const Vec2f mousePos) {
	float delta = mousePos.y - anchorPoint.y;
	collider.res.y = prevRes.y + delta;
}