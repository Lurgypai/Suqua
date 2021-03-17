#include "PacketHandler.h"

PacketHandler::PacketHandler(PacketHandlerId id_) :
	id{id_}
{}

PacketHandlerId PacketHandler::getId() const {
	return id;
}
