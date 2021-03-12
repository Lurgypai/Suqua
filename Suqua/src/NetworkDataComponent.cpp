#include "NetworkDataComponent.h"
#include <cstring>

NetworkDataComponent::NetworkDataComponent(EntityId id_) :
	id{ id_ },
	data_ {}
{}

bool NetworkDataComponent::Data::operator==(const NetworkDataComponent::Data& other) {
	return value == other.value && type == other.type;
}

bool NetworkDataComponent::Data::operator!=(const NetworkDataComponent::Data& other) {
	return !((*this) == other);
}

void NetworkDataComponent::serializeForNetwork(ByteStream& stream) {
	stream << data_.size();
	for (auto&& pair : data_) {
		stream << pair.first;
		pair.second.write(stream);
	}
}

void NetworkDataComponent::serializeForNetwork(ByteStream& stream, const std::map<DataId, Data>& prevData) {

	size_t writeCount = 0;
	for (auto&& pair : data_) {
		if (pair.second != prevData.at(pair.first)) {
			++writeCount;
		}
	}
	stream << writeCount;
	for (auto&& pair : data_) {
		if (pair.second != prevData.at(pair.first)) {
			stream << pair.first;
			pair.second.write(stream);
		}
	}
}

void NetworkDataComponent::unserialize(ByteStream& stream) {

	DataId id;
	size_t size;
	stream >> size;

	for (size_t i = 0; i != size; ++i) {
		if (stream >> id) {
			data_.at(id).read(stream);
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

const NetworkDataComponent::DataMap& NetworkDataComponent::data() {
	return data_;
}

EntityId NetworkDataComponent::getId() const {
	return id;
}