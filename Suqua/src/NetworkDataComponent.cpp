#include "NetworkDataComponent.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>
#include <exception>

EntityId NetworkDataComponent::GetEntityId(const UUID& id) {
    auto found = idMap.find(id);
    if(found == idMap.end()) return 0;
    return found->second;
}

void NetworkDataComponent::RemoveEntity(const UUID& id) {
    idMap.erase(id);
}

NetworkDataComponent::NetworkDataComponent(EntityId id_,
        const UUID& uuid_,
        NetworkDataComponent::Owner owner_) :
	id{ id_ },
    owner{ owner_ },
    uuid{ uuid_ },
	dataPtr{ new DataMap{} },
    prevDataPtr{ new DataMap{} }
{
    if(uuid == UUID{}) throw std::runtime_error{"Invalid UUID: Default UUID used"};

    idMap.emplace(uuid, id);
}

NetworkDataComponent::NetworkDataComponent(const NetworkDataComponent& other) :
	id{ other.id },
	dataPtr{new DataMap{ *other.dataPtr }},
    prevDataPtr{ new DataMap{ *other.dataPtr }}
{}

NetworkDataComponent& NetworkDataComponent::operator=(const NetworkDataComponent& other) {
	id = other.id;
	dataPtr = std::make_unique<DataMap>(*other.dataPtr);
    prevDataPtr = std::make_unique<DataMap>(*other.prevDataPtr);
	return *this;
}

bool NetworkDataComponent::Data::operator==(const NetworkDataComponent::Data& other) const {
	return value == other.value && type == other.type;
}

bool NetworkDataComponent::Data::operator!=(const NetworkDataComponent::Data& other) const {
	return !((*this) == other);
}

bool NetworkDataComponent::operator==(const NetworkDataComponent& other) const {
    return *dataPtr == *other.dataPtr && id == other.id;
}

bool NetworkDataComponent::operator!=(const NetworkDataComponent& other) const {
    return !(*this == other);
}

void NetworkDataComponent::serializeForNetwork(ByteStream& stream) {
    stream << uuid;
	//add the ability to allocate bytestream space, and overwrite at position
	size_t writeCount = 0;
	for (const auto& pair : *dataPtr) {
        // skip unchanged values
        auto prevPair = prevDataPtr->find(pair.first);
        if(prevPair != prevDataPtr->end() && prevPair->second == pair.second) continue;
        ++writeCount;
	}

	stream << writeCount;
	for (auto&& pair : *dataPtr) {
        // skip unchanged values
        auto prevPair = prevDataPtr->find(pair.first);
        if(prevPair != prevDataPtr->end() && prevPair->second == pair.second) continue;
        
        stream << pair.first;
        pair.second.write(stream);
	}
}

//void NetworkDataComponent::serializeForNetwork(ByteStream& stream, const std::map<DataId, Data>& prevData) {
//
//	size_t writeCount = 0;
//	for (auto&& pair : data_) {
//		if (pair.second != prevData.at(pair.first)) {
//			++writeCount;
//		}
//	}
//	stream << writeCount;
//	for (auto&& pair : data_) {
//		if (pair.second != prevData.at(pair.first)) {
//			stream << pair.first;
//			pair.second.write(stream);
//		}
//	}
//}


void NetworkDataComponent::unserialize(ByteStream& stream) {

	DataId dataId;
	size_t size;
	stream >> size;

	for (size_t i = 0; i != size; ++i) {
		if (stream >> dataId) {
			dataPtr->at(dataId).read(stream);
		}
		else {
			//we should have more elements to read
			throw std::exception{};
		}
	}
}

void NetworkDataComponent::MoveStreamPast(ByteStream& stream) {
    DataId id;
    DataType type;
    size_t size;
    stream >> size;
    for(size_t i = 0; i != size; ++i) {
        if(stream >> id) {
            stream >> type;
            switch(type) {
            case DataType::BOOL:
                stream.moveReadPos(sizeof(bool));
                break;
            case DataType::UBYTE:
                stream.moveReadPos(sizeof(std::uint8_t));
                break;
            case DataType::INT_32:
                stream.moveReadPos(sizeof(std::int32_t));
                break;
            case DataType::FLOAT:
                stream.moveReadPos(sizeof(float));
                break;
            case DataType::STRING:
                size_t strSize;
                stream >> strSize;
                stream.moveReadPos(strSize);
                break;
            default:
                break;
            }
        }
        else {
            throw std::exception{};
        }
    }
}

void NetworkDataComponent::Data::write(ByteStream& s) {
	s << static_cast<char>(type);
	switch (type)
	{
	case NetworkDataComponent::Data::DataType::UBYTE:
		s << get<uint8_t>();
		break;
	case NetworkDataComponent::Data::DataType::BOOL:
		s << get<bool>();
		break;
	case NetworkDataComponent::Data::DataType::INT_32:
		s << get<int32_t>();
		break;
	case NetworkDataComponent::Data::DataType::FLOAT:
		s << get<float>();
		break;
	case NetworkDataComponent::Data::DataType::STRING:
		s << get<std::string>();
		break;
	default:
		break;
	}
}

inline void NetworkDataComponent::Data::read(ByteStream& s) {
	s >> type;
	switch (type)
	{
	case NetworkDataComponent::Data::DataType::UBYTE:
		s >> get<uint8_t>();
		break;
	case NetworkDataComponent::Data::DataType::BOOL:
		s >> get<bool>();
		break;
	case NetworkDataComponent::Data::DataType::INT_32:
		s >> get<int32_t>();
		break;
	case NetworkDataComponent::Data::DataType::FLOAT:
		s >> get<float>();
		break;
	case NetworkDataComponent::Data::DataType::STRING:
		s >> get<std::string>();
		break;
	default:
		break;
	}
}

void NetworkDataComponent::storePrev() {
    for(auto& [id, data] : *dataPtr) {
        (*prevDataPtr)[id] = data;     
    }
}

void NetworkDataComponent::storePrev(DataId id) {
    (*prevDataPtr)[id] = dataPtr->at(id);
}

const UUID& NetworkDataComponent::getUUID() const {
    return uuid;
}

std::unordered_map<UUID, EntityId> NetworkDataComponent::idMap{};
