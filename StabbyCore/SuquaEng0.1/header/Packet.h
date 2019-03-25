#pragma once
#include "NetworkTypes.h"
#include "Controller.h"
#include "PlayerData.h"
#include "enet/enet.h"
#include "Vec2.h"
#include <string>
#define PACKET_KEY_SIZE 3	//will be plus oned in the packets to hold the null character.
#define WELCOME_KEY "SHI"	//suqua hi
#define CONT_KEY	"SCT"	//suqua control
#define STATE_KEY	"SST"	//suqua state
#define JOIN_KEY	"SJN"	//suqua join
#define QUIT_KEY	"SQT"	//suqua quit
#define TIME_KEY	"STS"	//suqua time stamp

//sent to confirm a connection. Sent from server to client
struct WelcomePacket {
	WelcomePacket() :
		key{WELCOME_KEY},
		currentTick{0},
		netId{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	NetworkId netId;
	//the serverside time
	Time_t currentTick;
};

//contains state information
struct ControllerPacket {
	ControllerPacket() :
		key{CONT_KEY},
		netId{0},
		time{0},
		state{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	NetworkId netId;
	//the client side time when the packet was sent
	Time_t time;
	//game time when the packet was sent
	Time_t when;
	unsigned char state;

	bool operator!=(const ControllerPacket & other) {
		return netId != other.netId || time != other.time || when != other.when || state != other.state;
	}
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

struct StatePacket {
	StatePacket() :
		key{ STATE_KEY },
		state{},
		id{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	//the player who moved
	NetworkId id;
	//game time when the packet was sent
	Time_t when;
	PlayerState state;
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

struct TimestampPacket {
	TimestampPacket() :
		key{ TIME_KEY },
		id{ 0 },
		clientTime{0},
		serverTime{0},
		gameTime{0}
	{}

	char key[PACKET_KEY_SIZE + 1];
	NetworkId id;
	//when the packet was sent from the server (in local client ticks)
	Time_t clientTime;
	//what time the packet was recieved on the server (in local server ticks)
	Time_t serverTime;
	//the game time according to the server
	Time_t gameTime;
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

	template<typename T>
	inline void readInto(T * t, ENetPacket * p, size_t count) {
		std::memcpy(t, p->data, sizeof(T) * count);
	}
}