#include "PacketHandler.h"

PacketHandler::PacketHandler(PacketId id_) :
	id{id_}
{}

PacketHandler::~PacketHandler() {};

PacketId PacketHandler::getId() const {
	return id;
}
