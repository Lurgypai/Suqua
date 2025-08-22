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
	dataMap{}
{
    if(uuid == UUID{}) throw std::runtime_error{"Invalid UUID: Default UUID used"};

    idMap.emplace(uuid, id);
}

bool NetworkDataComponent::Data::operator==(const NetworkDataComponent::Data& other) const {
	return value == other.value && type == other.type;
}

bool NetworkDataComponent::Data::operator!=(const NetworkDataComponent::Data& other) const {
	return !((*this) == other);
}

bool NetworkDataComponent::operator==(const NetworkDataComponent& other) const {
    return dataMap == other.dataMap && id == other.id && dataMap == other.dataMap;
}

bool NetworkDataComponent::operator!=(const NetworkDataComponent& other) const {
    return !(*this == other);
}



void NetworkDataComponent::serializeForNetwork(ByteStream& stream) {
    stream << uuid;
	stream << dataMap.size();
	for (auto&& pair : dataMap) {
        //write delta
        switch(pair.second.type) {
        case NetworkDataComponent::Data::DataType::UBYTE:
            writeDelta<std::uint8_t>(pair, stream);
            break;
        case NetworkDataComponent::Data::DataType::BOOL:
            writeDelta<bool>(pair, stream);
            break;
        case NetworkDataComponent::Data::DataType::INT_32:
            writeDelta<std::int32_t>(pair, stream);
            break;
        case NetworkDataComponent::Data::DataType::FLOAT:
            writeDelta<float>(pair, stream);
            break;
        case NetworkDataComponent::Data::DataType::STRING: {
            const std::string& localVal = *(std::get<std::string*>(pair.second.value));
            const std::string& prevVal = std::get<std::string>(prevDataMap.at(pair.first));
            // right now send all
            // if(localVal == prevVal) break;
            stream << pair.first;
            stream << *(std::get<std::string*>(pair.second.value));
            } break;
        default:
            break;
        }
	}
}

void NetworkDataComponent::unserialize(ByteStream& stream) {
	DataId dataId;
	size_t size;
	stream >> size;

	for (size_t i = 0; i != size; ++i) {
		if (stream >> dataId) {
            auto pair = dataMap.find(dataId);
            switch(pair->second.type) {
            case NetworkDataComponent::Data::DataType::UBYTE:
                readDelta<std::uint8_t>(*pair, stream);
                break;
            case NetworkDataComponent::Data::DataType::BOOL:
                readDelta<bool>(*pair, stream);
                break;
            case NetworkDataComponent::Data::DataType::INT_32:
                readDelta<std::int32_t>(*pair, stream);
                break;
            case NetworkDataComponent::Data::DataType::FLOAT:
                readDelta<float>(*pair, stream);
                break;
            case NetworkDataComponent::Data::DataType::STRING: {
                std::string& localVal = *(std::get<std::string*>(pair->second.value));
                const std::string& prevVal = std::get<std::string>(serverDataMap.at(pair->first));
                localVal = prevVal;
                } break;
            default:
                break;
            }
		}
		else {
			//we should have more elements to read
			throw std::exception{};
		}
	}

    storePrev(serverDataMap);
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

void NetworkDataComponent::storePrev(PrevDataMap& prev) {
    for(const auto& pair : dataMap) {
        switch(pair.second.type) {
        case NetworkDataComponent::Data::DataType::UBYTE:
            prev[pair.first] = *(std::get<std::uint8_t*>(pair.second.value));
            break;
        case NetworkDataComponent::Data::DataType::BOOL:
            prev[pair.first] = *(std::get<bool*>(pair.second.value));
            break;
        case NetworkDataComponent::Data::DataType::INT_32:
            prev[pair.first] = *(std::get<std::int32_t*>(pair.second.value));
            break;
        case NetworkDataComponent::Data::DataType::FLOAT:
            prev[pair.first] = *(std::get<float*>(pair.second.value));
            break;
        case NetworkDataComponent::Data::DataType::STRING:
            prev[pair.first] = *(std::get<std::string*>(pair.second.value));
            break;
        default:
            break;
        }
    }
}

const UUID& NetworkDataComponent::getUUID() const {
    return uuid;
}

std::unordered_map<UUID, EntityId> NetworkDataComponent::idMap{};
