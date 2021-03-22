#include "..\header\ClientSyncSystem.h"
#include "NetworkDataComponent.h"
#include "OnlineComponent.h"

void ClientSyncSystem::storeData(Tick gameTime)
{
	//store controller values

	ByteStream stream;
	stream << EntitySystem::GetPool<NetworkDataComponent>().size();
	for (auto&& data : EntitySystem::GetPool<NetworkDataComponent>()) {
		stream << data.getId();
		data.serializeForNetwork(stream);
	}
	states.emplace(gameTime, std::move(stream));
}

void ClientSyncSystem::readData(ByteStream& bytes) {
	//store previous NDC states

	Tick targetTime;
	bytes >> targetTime;
	ByteStream before = states.at(targetTime);

	//update all ndcs to the states provided in bytes
	//compare before and after
	//if unequal, reset to before, then run till current time
}
