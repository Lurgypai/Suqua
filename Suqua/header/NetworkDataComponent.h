#pragma once
#include <variant>
#include <cstdint>
#include <string>
#include <unordered_map>
#include "ByteStream.h"
#include "ByteOrder.h"
#include "EntitySystem.h"
#include <memory>

//where do you want to store previous states for interpolation?
//where do you want to store the SyncMode (none, immediate, interpolated)?

// as it is, the NDC is not cache friendly. A major overhaul would be needed to improve cache friendliness, with a custom backing structure, and packing of the data elements, to conserve space while keeping data local.

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
		using DataValue = std::variant<char, bool, uint32_t, int32_t, uint64_t, int64_t, float, double, std::string, Vec2f>;

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
			STRING,
			VEC2F,
		} type;
		SyncMode mode;

		inline Data() = default;
		/*
		inline Data(const Data& other) = default;
		inline Data(Data&& other) = default;

		Data& operator=(const Data& other) = default;
		Data& operator=(Data&& other) = default;
		*/

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
	NetworkDataComponent(NetworkDataComponent&& other) = default;
	NetworkDataComponent& operator=(NetworkDataComponent&& other) = default;
	NetworkDataComponent(const NetworkDataComponent& other);
    NetworkDataComponent& operator=(const NetworkDataComponent& other);

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
	using DataMap = std::unordered_map<DataId, Data>;
	using DataMapPtr = std::unique_ptr<DataMap>;

	DataMapPtr dataPtr;
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
	(*dataPtr)[id].set(t);
}

template<typename T>
inline void NetworkDataComponent::set(DataId id, T&& t) {
	(*dataPtr)[id].set(std::forward<T&&>(t));
}

template<typename T>
inline void NetworkDataComponent::set(DataId id, const T& t) {
	(*dataPtr)[id].set(t);
}

template<typename T>
T& NetworkDataComponent::get(DataId id) {
	return dataPtr->at(id).get<T>();
}

template<typename T>
inline const T& NetworkDataComponent::get(DataId id) const {
	return dataPtr->at(id).getConst<T>();
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

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<Vec2f>() {
	return NetworkDataComponent::Data::DataType::VEC2F;
}