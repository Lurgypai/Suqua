#include <cstring>
#include <stdexcept>
#include <utility>
#include <exception>
#include <format>

#include "NetworkDataComponent.h"

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
    size_t totalWritten = 0;
    size_t sizePos = stream.getPos();
    stream.allocateData(sizeof(size_t)); // allocate space for the size;
	for (auto&& pair : dataMap) {
        //write delta
        switch(pair.second.type) {
        case NetworkDataComponent::Data::DataType::UBYTE:
            if(writeDelta<std::uint8_t>(pair, stream)) ++totalWritten;
            break;
        case NetworkDataComponent::Data::DataType::BOOL:
            if(writeDelta<bool>(pair, stream)) ++totalWritten;
            break;
        case NetworkDataComponent::Data::DataType::INT_32:
            if(writeDelta<std::int32_t>(pair, stream)) ++totalWritten;
            break;
        case NetworkDataComponent::Data::DataType::FLOAT:
            if(writeDelta<float>(pair, stream)) ++totalWritten;
            break;
        case NetworkDataComponent::Data::DataType::STRING: {
            const std::string& localVal = *(std::get<std::string*>(pair.second.value));
            const std::string& prevVal = std::get<std::string>(prevDataMap.at(pair.first));
            if(localVal == prevVal) break;
            stream << pair.first;
            stream << pair.second.type;
            stream << *(std::get<std::string*>(pair.second.value));
            ++totalWritten;
            } break;
        default:
            break;
        }
	}

    size_t end = stream.getPos();
    stream.setPos(sizePos);
    stream << totalWritten;
    stream.setPos(end);
}

void NetworkDataComponent::unserialize(ByteStream& stream) {
	DataId dataId;
	size_t size;
	stream >> size;

	for (size_t i = 0; i != size; ++i) {
		if (stream >> dataId) {
            stream.movePos(sizeof(DataType)); // skip type value used by MoveStreamPast
            auto pair = dataMap.find(dataId);
            if(pair == dataMap.end()) {
                throw std::runtime_error{std::format("Unable to find data with id {}", dataId)}; }
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
                stream >> localVal;
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
                stream.movePos(sizeof(bool));
                break;
            case DataType::UBYTE:
                stream.movePos(sizeof(std::uint8_t));
                break;
            case DataType::INT_32:
                stream.movePos(sizeof(std::int32_t));
                break;
            case DataType::FLOAT:
                stream.movePos(sizeof(float));
                break;
            case DataType::STRING:
                size_t strSize;
                stream >> strSize;
                stream.movePos(strSize);
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

const UUID& NetworkDataComponent::getUUID() const {
    return uuid;
}

void NetworkDataComponent::storePrev() {
    for(auto& pair : dataMap) {
        switch(pair.second.type) {
            case DataType::BOOL:
                prevDataMap.at(pair.first) = *(std::get<bool*>(pair.second.value));
                break;
            case DataType::UBYTE:
                prevDataMap.at(pair.first) = *(std::get<uint8_t*>(pair.second.value));
                break;
            case DataType::INT_32:
                prevDataMap.at(pair.first) = *(std::get<int32_t*>(pair.second.value));
                break;
            case DataType::FLOAT:
                prevDataMap.at(pair.first) = *(std::get<float*>(pair.second.value));
                break;
            case DataType::STRING:
                prevDataMap.at(pair.first) = *(std::get<std::string*>(pair.second.value));
                break;
            default:
                break;
        }
    }
}

std::unordered_map<UUID, EntityId> NetworkDataComponent::idMap{};
