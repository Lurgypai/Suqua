#pragma once
#include "NetworkTypes.h"
#include "Controller.h"
#include "enet/enet.h"
#include <string>
#define PACKET_KEY_SIZE 3	//will be plus oned in the packets to hold the null character.
#define WELCOME_KEY "SHI"	//suqua hi
#define STATE_KEY	"SST"	//suqua state
#define JOIN_KEY	"SJN"	//suqua join
#define POS_KEY		"SPS"	//suqua pos
#define QUIT_KEY	"SQT"	//suqua quit

//sent to confirm a connection. Sent from server to client
struct WelcomePacket {
	WelcomePacket() :
		key{WELCOME_KEY},
		currentTime{0},
		netId{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	NetworkId netId;
	Time_t currentTime;
};

//contains state information
struct StatePacket {
	StatePacket() :
		key{STATE_KEY},
		netId{0},
		time{0},
		state{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	NetworkId netId;
	Time_t time;
	unsigned char state;
};

//sent to all previously connected clients when a new player joins
struct JoinPacket {
	
	JoinPacket() :
		key{JOIN_KEY},
		joinerId{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	//id of the new person
	NetworkId joinerId;
};

struct PositionPacket {
	PositionPacket() :
		key{ POS_KEY },
		x{ 0.0f },
		y{ 0.0f },
		id{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	//the player who moved
	NetworkId id;
	Time_t when;
	float x;
	float y;
};

struct QuitPacket {
	QuitPacket() :
		key{QUIT_KEY},
		id{0}
	{}
	char key[PACKET_KEY_SIZE + 1];
	//the one who quit
	NetworkId id;
};

namespace PacketUtil {
	inline std::string readPacketKey(ENetPacket * packet) {
		std::string s{};
		s.resize(PACKET_KEY_SIZE);
		std::memcpy(&s[0], packet->data, PACKET_KEY_SIZE);
		return s;
	}

	template<typename T>
	inline void readInto(T& t, ENetPacket * p) {
		std::memcpy(&t, p->data, sizeof(t));
	}
}