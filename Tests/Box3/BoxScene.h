#include "Scene.h"
#include "PhysicsSystem.h"

class BoxScene : public Scene {
public:
	BoxScene(SceneId id_);

	// Inherited via Scene
	virtual void load() override;
	virtual void prePhysicsStep(double physics_step) override;
	virtual void physicsStep(double physics_step) override;
	virtual void postPhysicsStep(double physics_step) override;
	virtual void preRenderStep(RenderSystem& render) override;
	virtual void renderStep(RenderSystem& render) override;
	virtual void postRenderStep(RenderSystem& render) override;
private:
	PhysicsSystem physics;
};