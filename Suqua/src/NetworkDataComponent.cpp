#include <print>

#include "NetworkDataComponent.h"
#include <cstring>
#include <memory>
#include <stdexcept>
#include <utility>
#include <exception>
#include <format>

using UUID = Suqua::UUID;

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
	dataPtr{ new DataMap{} }
{
    if(uuid == UUID{}) throw std::runtime_error{"Invalid UUID: Default UUID used"};

    idMap.emplace(uuid, id);
}

NetworkDataComponent::NetworkDataComponent(const NetworkDataComponent& other) :
	id{ other.id },
	dataPtr{new DataMap{ *other.dataPtr }}
{}

NetworkDataComponent& NetworkDataComponent::operator=(const NetworkDataComponent& other) {
	id = other.id;
	dataPtr = std::make_unique<DataMap>(*other.dataPtr);
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
	stream << dataPtr->size();
	for (auto&& pair : *dataPtr) {
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
		s << *(std::get<std::uint8_t*>(value));
		break;
	case NetworkDataComponent::Data::DataType::BOOL:
		s << *(std::get<bool*>(value));
		break;
	case NetworkDataComponent::Data::DataType::INT_32:
		s << *(std::get<std::int32_t*>(value));
		break;
	case NetworkDataComponent::Data::DataType::FLOAT:
		s << *(std::get<float*>(value));
		break;
	case NetworkDataComponent::Data::DataType::STRING:
		s << *(std::get<std::string*>(value));
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
		s >> *(std::get<std::uint8_t*>(value));
		break;
	case NetworkDataComponent::Data::DataType::BOOL:
		s >> *(std::get<bool*>(value));
		break;
	case NetworkDataComponent::Data::DataType::INT_32:
		s >> *(std::get<std::int32_t*>(value));
		break;
	case NetworkDataComponent::Data::DataType::FLOAT:
		s >> *(std::get<float*>(value));
		break;
	case NetworkDataComponent::Data::DataType::STRING:
		s >> *(std::get<std::string*>(value));
		break;
	default:
		break;
	}
}

const UUID& NetworkDataComponent::getUUID() const {
    return uuid;
}

std::unordered_map<UUID, EntityId> NetworkDataComponent::idMap{};
