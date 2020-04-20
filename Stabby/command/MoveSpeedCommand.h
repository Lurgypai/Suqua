#pragma once
#include "Command.h"
#include "../game/Game.h"

class MoveSpeedCommand : public Command {
public:
	MoveSpeedCommand(Game & game_);

	// Inherited via Command
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Game & game;
};