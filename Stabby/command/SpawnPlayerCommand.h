#pragma once
#include "Command.h"
#include "player.h"
#include "../game/Game.h"

class SpawnPlayerCommand : public Command {
public:
	SpawnPlayerCommand(Game* game);
	// Inherited via Command
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Game* game;
};