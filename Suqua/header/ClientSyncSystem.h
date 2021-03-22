#pragma once
#include "Tick.h"
#include <unordered_map>
#include "ByteStream.h"

//stores history, and gets ready to send it

class ClientSyncSystem {
public:
	void storeData(Tick gameTime);
	void readData(ByteStream& bytes);
	
private:
	std::unordered_map<Tick, ByteStream> states;
};