#pragma once
#include "Command.h"
#include "../game/Game.h"

class AttackSpeedCommand : public Command {
public:
	AttackSpeedCommand(Game & game);

	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Game & game;
};