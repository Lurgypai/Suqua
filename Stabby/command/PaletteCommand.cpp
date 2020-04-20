
#include "PaletteCommand.h"

PaletteCommand::PaletteCommand(PaletteManager* palettes_) :
	palettes{palettes_}
{}

std::string PaletteCommand::getTag() const {
	return "palette";
}

void PaletteCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		palettes->setCurrentPalette(args[1]);
	}
}
