#include "SaveStageCommand.h"
#include "DebugIO.h"

SaveStageCommand::SaveStageCommand(EditableSystem& editables_) :
	editables{ editables_ }
{}

std::string SaveStageCommand::getTag() const {
	return "save";
}

void SaveStageCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		editables.save(args[1]);
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
