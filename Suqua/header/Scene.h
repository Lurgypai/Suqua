#pragma once
#include <cstdint>
#include <memory>
#include "EntitySystem.h"
#include "GLRenderer.h"
#include <set>
#include <vector>
#include "RenderSystem.h"

using SceneId = uint64_t;

class Scene {

public:
	Scene(SceneId id_);
	virtual ~Scene() = 0 {};
	virtual void load() = 0;
	virtual void prePhysicsStep(double physics_step) = 0;
	virtual void physicsStep(double physics_step) = 0;
	virtual void postPhysicsStep(double physics_step) = 0;

	virtual void preRenderStep(RenderSystem& render) = 0;
	virtual void renderStep(RenderSystem& render) = 0;
	virtual void postRenderStep(RenderSystem& render) = 0;
	virtual void unload();

	void removeDeadEntites();
	void drawScene(RenderSystem& render) const;

	const std::set<EntityId>& getEntities() const;
	CamId getCamId() const;

protected:
	SceneId id;
	CamId camId;

	std::vector<EntityId> addEntities(unsigned int count);
	void removeEntities(const std::vector<EntityId>& entities);
private:
	std::set<EntityId> entities;
};

using ScenePtr = std::unique_ptr<Scene>;