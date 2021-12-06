#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <map>
#include "ByteStream.h"
#include "ByteOrder.h"
#include "EntitySystem.h"

//where do you want to store previous states for interpolation?
//where do you want to store the SyncMode (none, immediate, interpolated)?

class NetworkDataComponent {
public:
	enum class SyncMode : char {
		//the default is immediate, this is cheapest option to still allow syncing. To be changed?
		IMMEDIATE = 0,
		INTERPOLATED = 1,
		NONE = 2
	};

private:
	class Data {
	public:
		using DataValue = std::variant<char, bool, uint32_t, int32_t, uint64_t, int64_t, float, double, std::string>;

		enum class DataType : char {
			NONE,
			BOOL,
			BYTE,
			UINT_32,
			INT_32,
			UINT_64,
			INT_64,
			FLOAT,
			DOUBLE,
			STRING
		} type;
		SyncMode mode;

		inline Data() = default;

		template<typename T>
		T& get();

		template<typename T>
		const T& getConst() const;

		template<typename T>
		void set(T& t);

		template<typename T>
		void set(T&& t);

		template<typename T>
		void set(const T& t);

		void write(ByteStream& s);
		void read(ByteStream& s);

		bool operator==(const Data& other) const;
		bool operator!=(const Data& other) const;
	private:

		DataValue value;

		template<typename T>
		constexpr DataType getDataType();
	};

public:
	using DataId = uint32_t;

	NetworkDataComponent(EntityId id_ = 0);
	NetworkDataComponent(const NetworkDataComponent& other) = default;
    NetworkDataComponent& operator=(const NetworkDataComponent& other) = default;

    bool operator==(const NetworkDataComponent& other) const;
    bool operator!=(const NetworkDataComponent& other) const;

	void serializeForNetwork(ByteStream& stream);
	//void serializeForNetwork(ByteStream& stream, const std::map<DataId, Data>& prevData);
	void unserialize(ByteStream& stream);
	
	template<typename T>
	void set(DataId id, T& t);
	template<typename T>
	void set(DataId id, T&& t);
	template<typename T>
	void set(DataId id, const T& t);
	template<typename T>
	T& get(DataId id);
    template<typename T>
    const T& get(DataId id) const;

	void setSyncMode(DataId id, SyncMode mode);

	//sets this entity to the interpolation between the two targets
	void interp(const NetworkDataComponent& first, const NetworkDataComponent& second, float ratio);

	//const DataMap& data();

	EntityId getId() const;
private:
	using DataMap = std::map<DataId, Data>;

	DataMap data_;
	EntityId id;
};


template<typename T>
inline T& NetworkDataComponent::Data::get() {
	return std::get<T>(value);
}

template<typename T>
inline const T& NetworkDataComponent::Data::getConst() const {
	return std::get<T>(value);
}

template<typename T>
inline void NetworkDataComponent::Data::set(T& t) {
	type = getDataType<T>();
	value = t;
}

template<typename T>
inline void NetworkDataComponent::Data::set(T&& t) {
	type = getDataType<T>();
	value = std::move(t);
}

template<typename T>
inline void NetworkDataComponent::Data::set(const T& t) {
	type = getDataType<T>();
	value = t;
}

template<typename T>
inline void NetworkDataComponent::set(DataId id, T& t) {
	data_[id].set(t);
}

template<typename T>
inline void NetworkDataComponent::set(DataId id, T&& t) {
	data_[id].set(std::forward<T&&>(t));
}

template<typename T>
inline void NetworkDataComponent::set(DataId id, const T& t) {
	data_[id].set(t);
}

template<typename T>
T& NetworkDataComponent::get(DataId id) {
	return data_.at(id).get<T>();
}

template<typename T>
inline const T& NetworkDataComponent::get(DataId id) const {
	return data_.at(id).getConst<T>();
}

template<typename T>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType() {
	return NetworkDataComponent::Data::DataType::NONE;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<char>() {
	return NetworkDataComponent::Data::DataType::BYTE;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<bool>() {
	return NetworkDataComponent::Data::DataType::BOOL;
}
 
template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<uint32_t>() {
	return NetworkDataComponent::Data::DataType::UINT_32;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<int32_t>() {
	return NetworkDataComponent::Data::DataType::INT_32;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<uint64_t>() {
	return NetworkDataComponent::Data::DataType::UINT_64;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<int64_t>() {
	return NetworkDataComponent::Data::DataType::INT_64;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<float>() {
	return NetworkDataComponent::Data::DataType::FLOAT;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<double>() {
	return NetworkDataComponent::Data::DataType::DOUBLE;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<std::string>() {
	return NetworkDataComponent::Data::DataType::STRING;
}
