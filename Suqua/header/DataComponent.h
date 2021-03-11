#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <map>
#include "ByteStream.h"
#include "ByteOrder.h"

class DataComponent {
public:
	class Data {
	public:
		using DataValue = std::variant<char, uint32_t, int32_t, uint64_t, int64_t, double, std::string>;

		enum class DataType : char {
			NONE,
			BYTE,
			UINT_32,
			INT_32,
			UINT_64,
			INT_64,
			DOUBLE,
			STRING
		} type;

		template<typename T>
		T& get();

		template<typename T>
		void set(const T& t);

		template<typename T>
		void set(T&& t);

		void write(ByteStream& s);
		void read(ByteStream& s);

		bool operator==(const Data& other);
		bool operator!=(const Data& other);
	private:

		DataValue value;

		template<typename T>
		constexpr DataType getDataType();
	};

	using DataId = uint32_t;

	DataComponent() = default;
	DataComponent(const DataComponent& other) = default;

	void serializeForNetwork(ByteStream& stream);
	void serializeForNetwork(ByteStream& stream, const std::map<DataId, Data>& prevData);
	void unserialize(ByteStream& stream);

	template<typename T>
	void set(DataId id, const T& t);
	template<typename T>
	void set(DataId id, T&& t);
	template<typename T>
	T& get(DataId id);

	const std::map<DataId, Data>& data();
private:
	std::map<DataId, Data> data_;
};

template<typename T>
inline T& DataComponent::Data::get() {
	return std::get<T>(value);
}

template<typename T>
inline void DataComponent::Data::set(const T& t) {
	type = getDataType<T>();
	value = t;
}

template<typename T>
inline void DataComponent::Data::set(T&& t) {
	type = getDataType<T>();
	value = std::forward<T&&>(t);
}

template<typename T>
inline void DataComponent::set(DataId id, const T& t) {
	data_[id].set(t);
}

template<typename T>
void DataComponent::set(DataId id, T&& t) {
	data_[id].set(std::forward<T&&>(t));
}

template<typename T>
T& DataComponent::get(DataId id) {
	return data_.at(id).get<T>();
}

template<typename T>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType() {
	return DataComponent::Data::DataType::NONE;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<char>() {
	return DataComponent::Data::DataType::BYTE;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<uint32_t>() {
	return DataComponent::Data::DataType::UINT_32;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<int32_t>() {
	return DataComponent::Data::DataType::INT_32;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<uint64_t>() {
	return DataComponent::Data::DataType::UINT_64;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<int64_t>() {
	return DataComponent::Data::DataType::INT_64;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<double>() {
	return DataComponent::Data::DataType::DOUBLE;
}

template<>
constexpr inline DataComponent::Data::DataType DataComponent::Data::getDataType<std::string>() {
	return DataComponent::Data::DataType::STRING;
}