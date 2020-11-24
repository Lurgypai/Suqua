#pragma once
#include "EditableStageComponent.h"
#include <list>

class EditableSystem {
public:
	EditableSystem();
	void updateLogic(int camId);
	const std::list<EntityId>& getNewEditables() const;
	void clearNewEditables();
	void updateGfx();
	void loadStageImage(const std::string& stage);
	void save(const std::string& stage);
	void load(const std::string& stage);
	EntityId makeEditable(Vec2f pos, Vec2f res, StageElement type, bool defaultSpawn_);
	bool isEnabled;

	EntityId getStageImage() const;
private:
	EntityId bgImage;
	std::list<EntityId> toGenEditables;
};