#include "PHClientState.h"
#include "Game.h"
#include "NetworkDataComponent.h"
#include <cstdint>

PHClientState::PHClientState(PacketId id_, Scene* scene_) :
	PacketHandler{ id_ },
    scene{scene_}
{}

static inline void skipData(ByteStream& data) {
    NetworkDataComponent::DataId id;
    NetworkDataComponent::DataType type;

    size_t size;
    data >> size;
    for(size_t i = 0; i != size; ++i) {
        if (data >> id) {
           data >> type; 
            switch (type)
            {
            case NetworkDataComponent::DataType::BYTE:
                data.moveReadPos(sizeof(char));
                break;
            case NetworkDataComponent::DataType::BOOL:
                data.moveReadPos(sizeof(bool));
                break;
            case NetworkDataComponent::DataType::UINT_32:
                data.moveReadPos(sizeof(std::uint32_t));
                break;
            case NetworkDataComponent::DataType::INT_32:
                data.moveReadPos(sizeof(std::int32_t));
                break;
            case NetworkDataComponent::DataType::UINT_64:
                data.moveReadPos(sizeof(std::uint64_t));
                break;
            case NetworkDataComponent::DataType::INT_64:
                data.moveReadPos(sizeof(std::int64_t));
                break;
            case NetworkDataComponent::DataType::FLOAT:
                data.moveReadPos(sizeof(float));
                break;
            case NetworkDataComponent::DataType::DOUBLE:
                data.moveReadPos(sizeof(double));
                break;
            case NetworkDataComponent::DataType::STRING:
                size_t size;
                data >> size;
                data.moveReadPos(size);
                break;
            default:
                break;
            }
        }
        else throw std::exception{};
    }
}

void PHClientState::handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) {
    PacketId packetId;
    data >> packetId;

    NetworkId netId;

    NetworkDataComponent dummy;
    while(data.hasMoreData()) {
        data >> netId;

        EntityId id = game.online.getEntity(netId);
        if(id == 0) {
            skipData(data);
            continue;
        }

        NetworkDataComponent* ndc = EntitySystem::GetComp<NetworkDataComponent>(id);
        if(ndc) ndc->unserialize(data);
    }
}
