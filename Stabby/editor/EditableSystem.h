#pragma once
#include "EditableStageComponent.h"

class EditableSystem {
public:
	EditableSystem();
	void updateLogic(int camId);
	void updateGfx();
	void loadStageImage(const std::string& stage);
	void save(const std::string& stage);
	void load(const std::string& stage);
	EntityId makeEditable(Vec2f pos, Vec2f res, StageElement type, bool defaultSpawn_);
	bool isEnabled;

	EntityId getStageImage() const;
private:
	EntityId bgImage;
};