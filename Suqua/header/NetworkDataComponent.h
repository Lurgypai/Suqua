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
#include "UUID.h"
#include "DebugFIO.h"

// change to send only deltas
// track local delta, only 
//
// add prev state tracking
// when we apply a change from the server, reset our deltas
// deserialize state from server
//      calculate local delta by subtracting current state from previous state
//      remove local delta from server delta
//      apply server delta

template<typename T>
concept IsDataValueType = std::same_as<T, bool> || std::same_as<T, std::uint8_t> || std::same_as<T, int32_t> || std::same_as<T, float> || std::same_as<T, std::string>;

class NetworkDataComponent {
    CompMembers(NetworkDataComponent);

public:
    enum class Owner : char {
        none,
        foreign,
        local_shared,
        local_only
    };

private:
	class Data {
    friend class NetworkDataComponent;
	public:
		using DataValue = std::variant<bool*, std::uint8_t*, std::int32_t*, float*, std::string*>;

		enum class DataType : char {
			NONE,
			BOOL,
			UBYTE,
			INT_32,
			FLOAT,
			STRING,
		} type;

        template<IsDataValueType T>
        Data(T& t, DataType type);

		bool operator==(const Data& other) const;
		bool operator!=(const Data& other) const;

        //transforms T into a datatype from the enum
		template<IsDataValueType T>
		static constexpr DataType getDataType();
	private:
		DataValue value;
	};

public:
	using DataId = uint32_t;
    using DataType = Data::DataType;

    static EntityId GetEntityId(const Suqua::UUID& id);
    static void RemoveEntity(const Suqua::UUID& id);

	NetworkDataComponent(EntityId id_, 
            const Suqua::UUID& uuid_,
            Owner owner_ = Owner::local_only );
	NetworkDataComponent(NetworkDataComponent&& other) = default;
	NetworkDataComponent& operator=(NetworkDataComponent&& other) = default;
	NetworkDataComponent(const NetworkDataComponent& other) = default;
    NetworkDataComponent& operator=(const NetworkDataComponent& other) = default;

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

    // remove ptr from map
    void unset(DataId id);

    const Suqua::UUID& getUUID() const;
    Owner owner;

    void storePrev();
    
private:
	using DataMap = std::unordered_map<DataId, Data>;
    using PrevDataValue = std::variant<bool, std::uint8_t, int32_t, float, std::string>;
    using PrevDataMap = std::unordered_map<DataId, PrevDataValue>;

	DataMap dataMap;
    // last state sent to server
    PrevDataMap prevDataMap;

    // store the current state in the target map
    Suqua::UUID uuid;
    static std::unordered_map<Suqua::UUID, EntityId> idMap;

    template<IsDataValueType T>
    inline void hasChanged(const DataMap::value_type& pair, ByteStream& stream);

    // helper function for writing, returns true if written
    template<IsDataValueType T>
    inline bool writeDelta(const DataMap::value_type& pair, ByteStream& stream);

    // helper function for reading
    template<IsDataValueType T>
    inline void readDelta(const DataMap::value_type& pair, ByteStream& stream);
};

template<IsDataValueType T>
NetworkDataComponent::Data::Data(T& t, DataType type_) : value{&t}, type{type_}
{}


template<IsDataValueType T>
inline void NetworkDataComponent::set(DataId id, T& t) {
    dataMap.emplace(id, Data{t, NetworkDataComponent::Data::getDataType<T>()});
    prevDataMap.emplace(id, t);
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

template<IsDataValueType T>
inline bool NetworkDataComponent::writeDelta(const DataMap::value_type& pair, ByteStream& stream) {
    const T& localVal = *(std::get<T*>(pair.second.value));
    T& prevVal = std::get<T>(prevDataMap.at(pair.first));
    T delta = localVal - prevVal;

    if (delta == 0) return false;
    stream << pair.first;
    stream << pair.second.type;
    stream << delta;

    prevVal = localVal;
    return true;
}

template<IsDataValueType T>
inline void NetworkDataComponent::readDelta(const DataMap::value_type& pair, ByteStream& stream) {
    T serverDelta;
    stream >> serverDelta;
    T& localVal = *(std::get<T*>(pair.second.value));
    T& prevVal = std::get<T>(prevDataMap.at(pair.first));
    localVal += serverDelta;
    // store to prevent being sent
    prevVal = localVal;

    // DebugFIO::TimeOut("debug.log") << std::format("localVal {}\n", localVal);
    // DebugFIO::TimeOut("debug.log") << std::format("serverDelta {}\n", serverDelta);
    // DebugFIO::TimeOut("debug.log") << std::format("localVal, updated {}\n", localVal);
    /*
    T localDelta = localVal - prevVal;

    T trueDelta = serverDelta - localDelta;
    
    DebugFIO::TimeOut("debug.log") << std::format("localDelta {}\n", localDelta);
    DebugFIO::TimeOut("debug.log") << std::format("localVal {}\n", localVal);

    localVal += trueDelta;
    prevVal = localVal;

    DebugFIO::TimeOut("debug.log") << std::format("trueDelta {}\n", trueDelta);
    DebugFIO::TimeOut("debug.log") << std::format("prevVal {}\n", prevVal);
    */
}
