#include "../Box3Core/BoxScene.h"
#include "enet/enet.h"

class ServerBoxScene : public BoxScene {
public:
	ServerBoxScene(SceneId id_, Scene::FlagType flags);
	virtual void onConnect(Game& game, PeerId connectingId) override;
};