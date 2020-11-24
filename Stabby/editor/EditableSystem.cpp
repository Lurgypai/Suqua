
#include "EditableSystem.h"
#include "EntitySystem.h"
#include "EditableStageComponent.h"
#include "EditableColliderGC.h"
#include "SDL.h"
#include "GLRenderer.h"
#include "DebugIO.h"
#include <fstream>
#include <iostream>
#include <RenderComponent.h>
#include <EntityBaseComponent.h>
#include <PositionComponent.h>
#include <cstring>

EditableSystem::EditableSystem() :
	isEnabled{false}
{}

void EditableSystem::updateLogic(int camId) {
	if (!isEnabled)
		return;
	if (EntitySystem::Contains<EditableStageComponent>()) {
		for (auto& editable : EntitySystem::GetPool<EditableStageComponent>()) {
			editable.update(camId);
		}
	}


	Vec2i mousePos_;
	Uint32 mouseState = SDL_GetMouseState(&mousePos_.x, &mousePos_.y);

	Vec2f mousePos = GLRenderer::screenToWorld(mousePos_, camId);

	static bool prevButton1 = false;
	static bool prevButton2 = false;

	bool currButton1 = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT);
	bool currButton2 = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT);

	const Uint8 * state = SDL_GetKeyboardState(NULL);
	bool ctrl = state[SDL_SCANCODE_LSHIFT];

	if (ctrl && currButton1 != prevButton1 && currButton1) {
		EntityId editableId = makeEditable(mousePos, { 0, 0 }, StageElement::collideable, false);
		auto editable = EntitySystem::GetComp<EditableStageComponent>(editableId);
		editable->anchorPoint = mousePos;
		editable->state = EditableState::resizing;
		editable->prevPoint = mousePos;
		editable->prevRes = editable->collider.res;
		editable->anchorSide = AnchorSide::topright;
	}
	if (ctrl && currButton2 != prevButton2 && currButton2) {
		EntityId editableId = makeEditable(mousePos, { 0, 0 }, StageElement::climbable, false);
		auto editable = EntitySystem::GetComp<EditableStageComponent>(editableId);
		editable->anchorPoint = mousePos;
		editable->state = EditableState::resizing;
		editable->prevPoint = mousePos;
		editable->prevRes = editable->collider.res;
		editable->anchorSide = AnchorSide::topright;
	}

	prevButton1 = currButton1;
	prevButton2 = currButton2;
}

const std::list<EntityId>& EditableSystem::getNewEditables() const {
	return toGenEditables;
}

void EditableSystem::clearNewEditables() {
	toGenEditables.clear();
}

void EditableSystem::updateGfx() {
	if (EntitySystem::Contains<EditableColliderGC>()) {
		for (auto& editable : EntitySystem::GetPool<EditableColliderGC>()) {
			editable.update();
		}
	}
}

void EditableSystem::loadStageImage(const std::string& stage_) {

	std::string stage = "stage::" + stage_;
	EntitySystem::GenEntities(1, &bgImage);
	EntitySystem::MakeComps<RenderComponent>(1, &bgImage);
	EntitySystem::GetComp<RenderComponent>(bgImage)->loadDrawable<Sprite>(stage);
	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(bgImage);
	const auto& res = render->getDrawable<Sprite>()->getImgRes();
	render->getDrawable<Sprite>()->setDepth(0.5);
	EntitySystem::GetComp<PositionComponent>(bgImage)->pos = { -res.x / 2, -res.y };
}

void EditableSystem::save(const std::string& stage_) {
	std::string stage = Stage::folder + '/' + stage_ + ".stg";
	std::ofstream file{stage, std::ios::out | std::ios::trunc | std::ios::binary};
	if (!file.is_open()) {
		std::cout << "Unable to open file \"" << stage << "\"\n";
		return;
	}

	constexpr int size = sizeof(AABB) + sizeof(StageElement) + sizeof(bool);
	char data[size];
	for (auto& editable : EntitySystem::GetPool<EditableStageComponent>()) {
		std::memcpy(data, &editable.collider, sizeof(AABB));
		std::memcpy(data + sizeof(AABB), &editable.type, sizeof(StageElement));
		std::memcpy(data + sizeof(AABB) + sizeof(StageElement), &editable.defaultSpawn, sizeof(bool));

		file.write(data, size);
	}
}

void EditableSystem::load(const std::string& stage_) {

	loadStageImage(stage_);

	std::string stage = Stage::folder + '/' + stage_ + ".stg";

	std::ifstream file{ stage, std::ios::binary | std::ios::ate };
	if (!file.is_open()) {
		std::cout << "Unable to open file \"" << stage << "\"\n";
		return;
	}

	constexpr int blockSize = sizeof(AABB) + sizeof(StageElement) + sizeof(bool);
	std::streamsize fileSize = file.tellg();
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(fileSize);
	if (file.read(buffer.data(), fileSize)) {
		for (auto pos = 0; pos < fileSize; pos += blockSize) {
			AABB collider;
			StageElement type;
			bool defaultSpawn;
			std::memcpy(&collider, buffer.data() + pos, sizeof(AABB));
			std::memcpy(&type, buffer.data() + pos + sizeof(AABB), sizeof(StageElement));
			std::memcpy(&defaultSpawn, buffer.data() + pos + sizeof(AABB) + sizeof(StageElement), sizeof(bool));
			makeEditable(collider.pos, collider.res, type, defaultSpawn);
		}
	}
	else {
		std::cout << "Unable to open file \"" << stage << "\"\n";
	}
}

EntityId EditableSystem::makeEditable(Vec2f pos, Vec2f res, StageElement type, bool defaultSpawn_) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<EditableStageComponent>(1, &id);
	auto editable = EntitySystem::GetComp<EditableStageComponent>(id);
	editable->collider.pos = { pos.x, pos.y };
	editable->collider.res = res;
	editable->type = type;
	editable->defaultSpawn = defaultSpawn_;

	EntitySystem::MakeComps<EditableColliderGC>(1, &id);

	toGenEditables.push_back(id);

	return id;
}

EntityId EditableSystem::getStageImage() const {
	return bgImage;
}
