#include "DataComponent.h"
#include <cstring>

bool DataComponent::Data::operator==(const DataComponent::Data& other) {
	return value == other.value && type == other.type;
}

bool DataComponent::Data::operator!=(const DataComponent::Data& other) {
	return !((*this) == other);
}

void DataComponent::serializeForNetwork(ByteStream& stream) {
	stream << data_.size();
	for (auto&& pair : data_) {
		stream << pair.first;
		pair.second.write(stream);
	}
}

void DataComponent::serializeForNetwork(ByteStream& stream, const std::map<DataId, Data>& prevData) {

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

void DataComponent::unserialize(ByteStream& stream) {

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

void DataComponent::Data::write(ByteStream& s) {
	s << static_cast<char>(type);
	switch (type)
	{
	case DataComponent::Data::DataType::BYTE:
		s << get<char>();
		break;
	case DataComponent::Data::DataType::UINT_32:
		s << get<uint32_t>();
		break;
	case DataComponent::Data::DataType::INT_32:
		s << get<int32_t>();
		break;
	case DataComponent::Data::DataType::UINT_64:
		s << get<uint64_t>();
		break;
	case DataComponent::Data::DataType::INT_64:
		s << get<int64_t>();
		break;
	case DataComponent::Data::DataType::DOUBLE:
		s << get<double>();
		break;
	case DataComponent::Data::DataType::STRING:
		s << get<std::string>();
		break;
	default:
		break;
	}
}

inline void DataComponent::Data::read(ByteStream& s) {
	s >> type;
	switch (type)
	{
	case DataComponent::Data::DataType::BYTE:
		s >> get<char>();
		break;
	case DataComponent::Data::DataType::UINT_32:
		s >> get<uint32_t>();
		break;
	case DataComponent::Data::DataType::INT_32:
		s >> get<int32_t>();
		break;
	case DataComponent::Data::DataType::UINT_64:
		s >> get<uint64_t>();
		break;
	case DataComponent::Data::DataType::INT_64:
		s >> get<int64_t>();
		break;
	case DataComponent::Data::DataType::DOUBLE:
		s >> get<double>();
		break;
	case DataComponent::Data::DataType::STRING:
		s >> get<std::string>();
		break;
	default:
		break;
	}
}

const std::map<DataComponent::DataId, DataComponent::Data>& DataComponent::data() {
	return data_;
}