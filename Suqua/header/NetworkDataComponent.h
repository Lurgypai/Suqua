#pragma once
#include <algorithm>
#include <concepts>
#include <variant>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

#include "ComponentMacros.h"
#include "ByteStream.h"


//where do you want to store previous states for interpolation?
//where do you want to store the SyncMode (none, immediate, interpolated)?

// as it is, the NDC is not cache friendly. A major overhaul would be needed to improve cache friendliness, with a custom backing structure, and packing of the data elements, to conserve space while keeping data local.
template<typename T>
concept IsDataValueType = std::same_as<T, bool> || std::same_as<T, std::uint8_t> || std::same_as<T, int32_t> || std::same_as<T, float> || std::same_as<T, std::string>;

// new changes
//  add uuid
//  add owner
//  add shared

class NetworkDataComponent {
    CompMembers(NetworkDataComponent);

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
		using DataValue = std::variant<bool, std::uint8_t, int32_t, float, std::string>;

		enum class DataType : char {
			NONE,
			BOOL,
			UBYTE,
			INT_32,
			FLOAT,
			STRING,
		} type;
		SyncMode mode;

		inline Data() = default;
		/*
		inline Data(const Data& other) = default;
		inline Data(Data&& other) = default;

		Data& operator=(const Data& other) = default;
		Data& operator=(Data&& other) = default;
		*/

		template<IsDataValueType T>
		T& get();

		template<IsDataValueType T>
		const T& getConst() const;

		template<IsDataValueType T>
		void set(T& t);

		template<IsDataValueType T>
		void set(T&& t);

		template<IsDataValueType T>
		void set(const T& t);

		void write(ByteStream& s);
		void read(ByteStream& s);

		bool operator==(const Data& other) const;
		bool operator!=(const Data& other) const;
	private:

		DataValue value;

		template<IsDataValueType T>
		constexpr DataType getDataType();
	};

public:
	using DataId = uint32_t;
    using DataType = Data::DataType;

	NetworkDataComponent(EntityId id_);
	NetworkDataComponent(NetworkDataComponent&& other) = default;
	NetworkDataComponent& operator=(NetworkDataComponent&& other) = default;
	NetworkDataComponent(const NetworkDataComponent& other);
    NetworkDataComponent& operator=(const NetworkDataComponent& other);

    // NOTE
    // this doesn't compare previous states
    // I don't think it needs to but if a bug comes up with weird ndc behaviour see this note please
    bool operator==(const NetworkDataComponent& other) const;
    bool operator!=(const NetworkDataComponent& other) const;

	void serializeForNetwork(ByteStream& stream);
	//void serializeForNetwork(ByteStream& stream, const std::map<DataId, Data>& prevData);
	void unserialize(ByteStream& stream);
    // skip an ndc in the stream without reading
    static void MoveStreamPast(ByteStream& stream);
	
	template<IsDataValueType T>
	void set(DataId id, T& t);
	template<IsDataValueType T>
	void set(DataId id, T&& t);
	template<IsDataValueType T>
	void set(DataId id, const T& t);
	template<IsDataValueType T>
	T& get(DataId id);
    template<IsDataValueType T>
    const T& get(DataId id) const;

	void setSyncMode(DataId id, SyncMode mode);

	//sets this entity to the interpolation between the two targets
	// void interp(const NetworkDataComponent& first, const NetworkDataComponent& second, float ratio);

	//const DataMap& data();

    // stores the current data in the prevDataPtr
    void storePrev();
    void storePrev(DataId field);
private:
	using DataMap = std::unordered_map<DataId, Data>;
	using DataMapPtr = std::unique_ptr<DataMap>;

	DataMapPtr dataPtr;
    DataMapPtr prevDataPtr;
};


template<IsDataValueType T>
inline T& NetworkDataComponent::Data::get() {
	return std::get<T>(value);
}

template<IsDataValueType T>
inline const T& NetworkDataComponent::Data::getConst() const {
	return std::get<T>(value);
}

template<IsDataValueType T>
inline void NetworkDataComponent::Data::set(T& t) {
	type = getDataType<T>();
	value = t;
}

template<IsDataValueType T>
inline void NetworkDataComponent::Data::set(T&& t) {
	type = getDataType<T>();
	value = std::move(t);
}

template<IsDataValueType T>
inline void NetworkDataComponent::Data::set(const T& t) {
	type = getDataType<T>();
	value = t;
}

template<IsDataValueType T>
inline void NetworkDataComponent::set(DataId id, T& t) {
	(*dataPtr)[id].set(t);
}

template<IsDataValueType T>
inline void NetworkDataComponent::set(DataId id, T&& t) {
	(*dataPtr)[id].set(std::forward<T&&>(t));
}

template<IsDataValueType T>
inline void NetworkDataComponent::set(DataId id, const T& t) {
	(*dataPtr)[id].set(t);
}

template<IsDataValueType T>
T& NetworkDataComponent::get(DataId id) {
	return dataPtr->at(id).get<T>();
}

template<IsDataValueType T>
inline const T& NetworkDataComponent::get(DataId id) const {
	return dataPtr->at(id).getConst<T>();
}

template<IsDataValueType T>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType() {
	return NetworkDataComponent::Data::DataType::NONE;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<uint8_t>() {
	return NetworkDataComponent::Data::DataType::UBYTE;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<bool>() {
	return NetworkDataComponent::Data::DataType::BOOL;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<int32_t>() {
	return NetworkDataComponent::Data::DataType::INT_32;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<float>() {
	return NetworkDataComponent::Data::DataType::FLOAT;
}

template<>
constexpr inline NetworkDataComponent::Data::DataType NetworkDataComponent::Data::getDataType<std::string>() {
	return NetworkDataComponent::Data::DataType::STRING;
}

