
#pragma once
#include "Scene.h"

class TestScene3 : public Scene {
public:
    TestScene3(SceneId id_, FlagType flags_);
    virtual ~TestScene3();
    virtual void load(Game& game);
    virtual void prePhysicsStep(Game& game);
    virtual void physicsStep(Game& game);
    virtual void postPhysicsStep(Game& game);

    virtual void preRenderStep(Game& game);
    virtual void renderStep(Game& game);
    virtual void postRenderStep(Game& game);
    virtual void unload(Game& game);

    virtual void onConnect(Game& game, PeerId connectingId) override;

    EntityId entity;
};
