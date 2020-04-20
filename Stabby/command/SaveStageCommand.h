#pragma once
#include "Command.h"
#include "../editor/EditableSystem.h"

class SaveStageCommand : public Command {
public:
	SaveStageCommand(EditableSystem& editables_);
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;

private:
	EditableSystem& editables;
};