#include "Scene.h"

class ConnectScene : public Scene {
public:
	ConnectScene(SceneId id_, FlagType flags_, const std::string& ip_, int port_);

	virtual void load(Game& game) override;
	virtual void prePhysicsStep(Game& game) override;
	virtual void physicsStep(Game& game) override;
	virtual void postPhysicsStep(Game& game) override;
	virtual void preRenderStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void postRenderStep(Game& game) override;
	virtual void unload(Game& game) override;
private:
	std::string ip;
	int port;
};