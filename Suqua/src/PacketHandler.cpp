#include "PacketHandler.h"

PacketHandler::PacketHandler(PacketHandlerId id_) :
	id{id_}
{}

PacketHandler::~PacketHandler() {};

PacketHandlerId PacketHandler::getId() const {
	return id;
}
