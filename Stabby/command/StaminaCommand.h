#pragma once
#include "Command.h"
#include "../game/Game.h"

class StaminaCommand : public Command {
public:
	StaminaCommand(Game& game);

	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Game& game;
};