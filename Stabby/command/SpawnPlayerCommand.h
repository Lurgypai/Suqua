#pragma once
#include "Command.h"
#include "player.h"

class SpawnPlayerCommand : public Command {
public:
	SpawnPlayerCommand(PlayerManager* players_, WeaponManager* weapons_);
	// Inherited via Command
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	PlayerManager* players;
	WeaponManager* weapons;
};