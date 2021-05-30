#pragma once
#include <cstdint>
#include <memory>
#include "EntitySystem.h"
#include "GLRenderer.h"
#include <set>
#include <vector>
#include "RenderSystem.h"
#include "InputDevice.h"

using SceneId = uint64_t;

class Game;

class Scene {

public:
	using FlagType = char;
	enum Flag : FlagType {
		render = 1 << 0,
		physics = 1 << 1,
		input = 1 << 2,
		all = render | physics | input,
		none = 0
	};

	Scene(SceneId id_, FlagType flags_);
	virtual ~Scene() = 0;
	virtual void load(Game& game) = 0;
	virtual void prePhysicsStep(Game& game) = 0;
	virtual void physicsStep(Game& game) = 0;
	virtual void postPhysicsStep(Game& game) = 0;

	virtual void preRenderStep(Game& game) = 0;
	virtual void renderStep(Game& game) = 0;
	virtual void postRenderStep(Game& game) = 0;
	virtual void unload(Game& game) = 0;

	void applyInputs(Game& game);
	void removeAllEntities();
	void removeDeadEntities();
	void drawScene(const RenderSystem& render) const;

	const std::set<EntityId>& getEntities() const;
	CamId getCamId() const;
	SceneId getId() const;

	char flags;
protected:
	using EntityInputSet = std::set<std::pair<EntityId, InputDeviceId>>;

	SceneId id;
	CamId camId;

	std::vector<EntityId> addEntities(unsigned int count);
	void removeEntities(const std::vector<EntityId>& entities);
	void addEntityInputs(const EntityInputSet& inputs);
	void removeEntityInputs(const EntityInputSet& inputs);
private:
	std::set<EntityId> entities;
	EntityInputSet entityInputs;
};

using ScenePtr = std::unique_ptr<Scene>;
