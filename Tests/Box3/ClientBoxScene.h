#include "../Box3Core/BoxScene.h"
#include "InputDevice.h"

class ClientBoxScene : public BoxScene {
public:
	ClientBoxScene(SceneId id_, FlagType flags_, InputDeviceId input_);
	virtual void load(Game& game) override;
	virtual void renderStep(Game& game) override;
private:
	InputDeviceId input;
};