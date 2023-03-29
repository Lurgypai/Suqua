#include "NetworkDataComponent.h"
#include <cstring>
#include <iostream>

NetworkDataComponent::NetworkDataComponent(EntityId id_) :
	id{ id_ },
	dataPtr{ new DataMap{} }
{}

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
	//add the ability to allocate bytestream space, and overwrite at position
	size_t writeCount = 0;
	for (const auto& pair : *dataPtr) {
		if (pair.second.mode != SyncMode::NONE) {
			++writeCount;
		}
	}

	stream << writeCount;
	for (auto&& pair : *dataPtr) {
		if (pair.second.mode != SyncMode::NONE) {
			stream << pair.first;
			pair.second.write(stream);
		}
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

	DataId id;
	size_t size;
	stream >> size;

	for (size_t i = 0; i != size; ++i) {
		if (stream >> id) {
			dataPtr->at(id).read(stream);
		}
		else {
			//we should have more elements to read
			throw std::exception{};
		}
	}
}

void NetworkDataComponent::Data::write(ByteStream& s) {
	s << static_cast<char>(type);
	switch (type)
	{
	case NetworkDataComponent::Data::DataType::BYTE:
		s << get<char>();
		break;
	case NetworkDataComponent::Data::DataType::BOOL:
		s << get<bool>();
		break;
	case NetworkDataComponent::Data::DataType::UINT_32:
		s << get<uint32_t>();
		break;
	case NetworkDataComponent::Data::DataType::INT_32:
		s << get<int32_t>();
		break;
	case NetworkDataComponent::Data::DataType::UINT_64:
		s << get<uint64_t>();
		break;
	case NetworkDataComponent::Data::DataType::INT_64:
		s << get<int64_t>();
		break;
	case NetworkDataComponent::Data::DataType::FLOAT:
		s << get<float>();
		break;
	case NetworkDataComponent::Data::DataType::DOUBLE:
		s << get<double>();
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
	case NetworkDataComponent::Data::DataType::BYTE:
		s >> get<char>();
		break;
	case NetworkDataComponent::Data::DataType::BOOL:
		s >> get<bool>();
		break;
	case NetworkDataComponent::Data::DataType::UINT_32:
		s >> get<uint32_t>();
		break;
	case NetworkDataComponent::Data::DataType::INT_32:
		s >> get<int32_t>();
		break;
	case NetworkDataComponent::Data::DataType::UINT_64:
		s >> get<uint64_t>();
		break;
	case NetworkDataComponent::Data::DataType::INT_64:
		s >> get<int64_t>();
		break;
	case NetworkDataComponent::Data::DataType::FLOAT:
		s >> get<float>();
		break;
	case NetworkDataComponent::Data::DataType::DOUBLE:
		s >> get<double>();
		break;
	case NetworkDataComponent::Data::DataType::STRING:
		s >> get<std::string>();
		break;
	default:
		break;
	}
}

//const NetworkDataComponent::DataMap& NetworkDataComponent::data() {
//	return data_;
//}

EntityId NetworkDataComponent::getId() const {
	return id;
}

void NetworkDataComponent::setSyncMode(DataId id, SyncMode mode) {
	dataPtr->at(id).mode = mode;
}

void NetworkDataComponent::interp(const NetworkDataComponent& start, const NetworkDataComponent& end, float ratio) {
	for (auto&& pair : *dataPtr) {
		if (pair.second.mode == SyncMode::INTERPOLATED) {
			switch (pair.second.type)
			{
			case NetworkDataComponent::Data::DataType::BYTE:
				pair.second.get<char>() = start.get<char>(pair.first) + (end.get<char>(pair.first) - start.get<char>(pair.first)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::UINT_32:
				pair.second.get<uint32_t>() = start.get<uint32_t>(pair.first) + (end.get<uint32_t>(pair.first) - start.get<uint32_t>(pair.first)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::INT_32:
				pair.second.get<int32_t>() = start.get<int32_t>(pair.first) + (end.get<int32_t>(pair.first) - start.get<int32_t>(pair.first)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::UINT_64:
				pair.second.get<uint64_t>() = start.get<uint64_t>(pair.first) + (end.get<uint64_t>(pair.first) - start.get<uint64_t>(pair.first)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::INT_64:
				pair.second.get<int64_t>() = start.get<int64_t>(pair.first) + (end.get<int64_t>(pair.first) - start.get<int64_t>(pair.first)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::FLOAT:
				//std::cout << "start value: " << start.get<float>(pair.first) << ", end value: " << end.get<float>(pair.first) << ", interpolated value " << start.get<float>(pair.first) + (end.get<float>(pair.first) - start.get<float>(pair.first)) * ratio << '\n';
				pair.second.get<float>() = start.get<float>(pair.first) + (end.get<float>(pair.first) - start.get<float>(pair.first)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::DOUBLE:
				pair.second.get<double>() = start.get<double>(pair.first) + (end.get<double>(pair.first) - start.get<double>(pair.first)) * ratio;
				break;
			default:
				break;
			}
		}
	}
}
