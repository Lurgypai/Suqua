#pragma once
#include "../Box3Core/BoxScene.h"
#include "InputDevice.h"

class ClientBoxScene : public BoxScene {
public:
	ClientBoxScene(SceneId id_, FlagType flags_, InputDeviceId input_, const std::string& ip_, int port_);
	virtual void load(Game& game) override;
	virtual void renderStep(Game& game) override;

	void linkInputToEntity(EntityId targetEntity);

	//the network id of the box this client owns
	NetworkId ourBoxNetId;
private:
	std::string ip;
	int port;
	InputDeviceId input;
};