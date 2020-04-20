#include "LoadStageCommand.h"
#include <DebugIO.h>
#include <EntityBaseComponent.h>

LoadStageCommand::LoadStageCommand(EditableSystem& editables_) :
	editables{editables_}
{}

std::string LoadStageCommand::getTag() const {
	return "load";
}

void LoadStageCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
			entity.isDead = true;
		}
		editables.load(args[1]);
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
