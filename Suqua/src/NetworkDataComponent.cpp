#include "NetworkDataComponent.h"
#include <cstring>
#include <iostream>

NetworkDataComponent::NetworkDataComponent(EntityId id_) :
	id{ id_ },
	data_ {}
{}

bool NetworkDataComponent::Data::operator==(const NetworkDataComponent::Data& other) const {
	return value == other.value && type == other.type;
}

bool NetworkDataComponent::Data::operator!=(const NetworkDataComponent::Data& other) const {
	return !((*this) == other);
}

bool NetworkDataComponent::operator==(const NetworkDataComponent& other) const {
    return data_ == other.data_ && id == other.id;
}

bool NetworkDataComponent::operator!=(const NetworkDataComponent& other) const {
    return !(*this == other);
}

void NetworkDataComponent::serializeForNetwork(ByteStream& stream) {
	//add the ability to allocate bytestream space, and overwrite at position
	size_t writeCount = 0;
	for (const auto& value : data_) {
		if (value.mode != SyncMode::NONE) {
			++writeCount;
		}
	}

	DataId pos = 0;
	stream << writeCount;
	for (auto&& value : data_) {
		if (value.mode != SyncMode::NONE) {
			stream << pos;
			value.write(stream);
		}
		++pos;
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
			data_[id].read(stream);
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
	data_[id].mode = mode;
}

void NetworkDataComponent::interp(const NetworkDataComponent& start, const NetworkDataComponent& end, float ratio) {
	DataId pos;
	for (auto&& value : data_) {
		if (value.mode == SyncMode::INTERPOLATED) {
			switch (value.type)
			{
			case NetworkDataComponent::Data::DataType::BYTE:
				value.get<char>() = start.get<char>(pos) + (end.get<char>(pos) - start.get<char>(pos)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::UINT_32:
				value.get<uint32_t>() = start.get<uint32_t>(pos) + (end.get<uint32_t>(pos) - start.get<uint32_t>(pos)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::INT_32:
				value.get<int32_t>() = start.get<int32_t>(pos) + (end.get<int32_t>(pos) - start.get<int32_t>(pos)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::UINT_64:
				value.get<uint64_t>() = start.get<uint64_t>(pos) + (end.get<uint64_t>(pos) - start.get<uint64_t>(pos)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::INT_64:
				value.get<int64_t>() = start.get<int64_t>(pos) + (end.get<int64_t>(pos) - start.get<int64_t>(pos)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::FLOAT:
				//std::cout << "start value: " << start.get<float>(pos) << ", end value: " << end.get<float>(pos) << ", interpolated value " << start.get<float>(pos) + (end.get<float>(pos) - start.get<float>(pos)) * ratio << '\n';
				value.get<float>() = start.get<float>(pos) + (end.get<float>(pos) - start.get<float>(pos)) * ratio;
				break;
			case NetworkDataComponent::Data::DataType::DOUBLE:
				value.get<double>() = start.get<double>(pos) + (end.get<double>(pos) - start.get<double>(pos)) * ratio;
				break;
			default:
				break;
			}
		}
		++pos;
	}
}
