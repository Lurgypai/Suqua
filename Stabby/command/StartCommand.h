#pragma once
#include "Command.h"
#include "../game/Game.h"

class StartCommand : public Command {
public:
	StartCommand(Game & game_);
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	Game & game;
};