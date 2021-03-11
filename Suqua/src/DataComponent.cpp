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
	s << type;
	s << value;
}

inline void DataComponent::Data::read(ByteStream& s) {
	s >> type;
	s >> value;
}

const std::map<DataComponent::DataId, DataComponent::Data>& DataComponent::data() {
	return data_;
}