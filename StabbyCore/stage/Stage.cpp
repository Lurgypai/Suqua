#include <fstream>
#include <cstring>

#include "Stage.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "ClimbableComponent.h"
#include "PhysicsComponent.h"
#include "../player/spawn/SpawnComponent.h"

const std::string Stage::folder{"stage"};

Stage::Stage() {}

Stage::Stage(const std::string& stage, SpawnSystem & spawns) :
	name{stage}
{
	std::string fileName = folder + '/' + name + ".stg";
	std::ifstream file{ fileName, std::ios::binary | std::ios::ate };
	if (!file.is_open()) {
		std::cout << "Unable to open file \"" << fileName << "\"\n";
		return;
	}

	struct SpawnBox {
		AABB box;
		bool defaultSpawn;
	};

	std::vector<SpawnBox> spawnBoxes{};

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
			
			switch (type)
			{
			case StageElement::collideable:
				EntityId id;
				EntitySystem::GenEntities(1, &id);
				EntitySystem::MakeComps<PhysicsComponent>(1, &id);
				EntitySystem::GetComp<PhysicsComponent>(id)->collideable = true;
				EntitySystem::GetComp<PhysicsComponent>(id)->weightless = true;
				EntitySystem::GetComp<PhysicsComponent>(id)->setRes(collider.res);
				EntitySystem::GetComp<PositionComponent>(id)->pos = collider.pos;
				collideables.push_back(id);
				break;
			case StageElement::climbable:
				EntityId ladder;
				EntitySystem::GenEntities(1, &ladder);
				EntitySystem::MakeComps<ClimbableComponent>(1, &ladder);
				EntitySystem::GetComp<ClimbableComponent>(ladder)->collider = collider;
				EntitySystem::GetComp<PositionComponent>(ladder)->pos = collider.pos;

				climbables.push_back(ladder);
				break;
			case StageElement::spawnable:
				spawnBoxes.emplace_back(SpawnBox{ collider, defaultSpawn });
			default:
				break;
			}
		}
		for (auto& box : spawnBoxes) {
			spawnZones.push_back(spawns.addSpawnZone(box.box, box.defaultSpawn, 0));
		}
	}
	else {
		std::cout << "Unable to open file \"" << stage << "\"\n";
	}
}

Vec2f Stage::getSpawnPos() const {
	return Vec2f{0, -10};
}

void Stage::loadGraphics() {
	renderables = { 0 };
	EntitySystem::GenEntities(1, renderables.data());
	EntitySystem::MakeComps<RenderComponent>(renderables.size(), renderables.data());

	for (auto& id : renderables) {
		RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);

		render->loadDrawable<Sprite>("stage::" + name);
		Sprite* sprite = render->getDrawable<Sprite>();
		render->offset = {sprite->getImgRes().x / 2, sprite->getImgRes().y};
		sprite->setDepth(0.9);
	}
}

const std::vector<EntityId>& Stage::getCollideables() const {
	return collideables;
}

const std::vector<EntityId>& Stage::getClimbables() const {
	return climbables;
}

const std::vector<EntityId>& Stage::getRenderables() const {
	return renderables;
}

const std::vector<EntityId>& Stage::getSpawnables() const {
	return spawnZones;
}