#pragma once
#include <cstdint>
#include <memory>
#include <unordered_map>
#include "EntitySystem.h"
#include "GLRenderer.h"
#include <set>
#include <vector>
#include "RenderSystem.h"
#include "InputDevice.h"
#include "Host.h"
#include "Tick.h"
#include "Controller.h"

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
	virtual void physicsStep(Game& game) = 0;
	virtual void renderUpdateStep(Game& game) = 0;

	virtual void renderStep(Game& game) = 0;
	virtual void unload(Game& game) = 0;

	virtual void onConnect(Game& game, PeerId connectingId) = 0;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) = 0;

	void doInputs(Game& game);

	// pointer to imply maybe existence
	// it might not exist if the scene doesn't have any current entities that need input
	const std::unordered_map<EntityId, Controller>* getInputsAtTime(Tick time) const;


	void removeAllEntities();
	void removeDeadEntities();
	void drawScene(const RenderSystem& render) const;

	const std::set<EntityId>& getEntities() const;
	CamId getCamId() const;
	SceneId getId() const;

	char flags;

	std::vector<EntityId> addEntities(unsigned int count);

	using EntityInputSet = std::set<std::pair<EntityId, InputDeviceId>>;
	using EntityInputMap = std::unordered_map<EntityId, InputDeviceId>;

	void addEntityInputs(const EntityInputSet& inputs);
	void removeEntityInputs(EntityId id);
protected:

	SceneId id;
	CamId camId;

	void removeEntities(const std::vector<EntityId>& entities);
private:
	void storeInputs(Game& game);
	void applyInputs(Game& game);

	std::set<EntityId> entities;
	EntityInputMap entityInputs;

	// entity input states to be applied at a later date.
	std::unordered_map<Tick, std::unordered_map<EntityId, Controller>> futureEntityInputs;
};

using ScenePtr = std::unique_ptr<Scene>;
