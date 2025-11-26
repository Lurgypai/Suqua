#include <fstream>
#include <random>

#include "nlohmann/json.hpp"

#include "SuquaLib.h"
#include "DebugFIO.h"

#include "Scene.h"
#include "PacketHandler.h"

using json = nlohmann::json;

constexpr unsigned int SECS_PER_FRAME{1};

class BenchmarkScene : public Scene {
public:
    BenchmarkScene(SceneId id_, Scene::FlagType flags_) : Scene{id_, flags_} {};
	// Inherited via Scene
	virtual void load(Game& game) override {};
	virtual void physicsStep(Game& game) override {
        int currBenchFrame = (game.getGameTick() / (game.TICK_RATE * SECS_PER_FRAME)) + 1;

        int amount = 1024 * currBenchFrame - (sizeof(Tick) + sizeof(int));

        std::random_device d{};
        std::default_random_engine e{d()};
        std::uniform_int_distribution<unsigned char> dst{0, 255};

        ByteStream packet;
        PacketId id{0};
        packet << id;
        packet << game.getGameTick();
        for(int i = 0; i != amount; ++i) {
            packet << dst(e);
        }
        game.host.bufferAllDataByChannel(0, packet);
    };
	virtual void renderUpdateStep(Game& game) override {};
	virtual void renderStep(Game& game) override {};
	virtual void unload(Game& game) override {};
	virtual void onConnect(Game& game, PeerId connectingId) override {};
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override {};

private:
};

class ClientPacketHandler : public PacketHandler {
public:
	ClientPacketHandler(PacketId id_) : PacketHandler{id_} {};
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override {
        PacketId id;
        data >> id;

        Tick sourceTime;
        data >> sourceTime;

        int sourceFrame = sourceTime / (game.TICK_RATE * SECS_PER_FRAME) + 1;
        Tick elapsed = game.getGameTick() - sourceTime;
        double elapsedTimeS = elapsed / game.TICK_RATE;
        std::cout << sourceFrame << "KiB Packet RTT: " << elapsedTimeS << "s\n";
    };
};

class ServerScene : public Scene {
public:
	// Inherited via Scene
	virtual void load(Game& game) override;
	virtual void physicsStep(Game& game) override {};
	virtual void renderUpdateStep(Game& game) override;
	virtual void renderStep(Game& game) override;
	virtual void unload(Game& game) override;
	virtual void onConnect(Game& game, PeerId connectingId) override;
	virtual void onDisconnect(Game& game, PeerId disconnectedPeer) override;

private:
};

class ServerPacketHandler : public PacketHandler {
public:
    ServerPacketHandler(PacketId id_) : PacketHandler{id_} {};
	virtual void handlePacket(Game& game, ByteStream& data, PeerId sourcePeer) override {
        game.host.bufferDataToChannel(sourcePeer, 0, data);
    };
};

int main(int argc, char** argv) {
	SuquaLib::SuquaInit("Shooty2", "settings.json", SuquaLib::network);

	json settings;
	std::ifstream file{ "settings.json" };
	if (file.good()) {
		file >> settings;
		file.close();
	}

	std::string ip = "127.0.0.1";
	if (settings.contains("ip")) {
		ip = settings["ip"];
	}

    std::string mode;
    if(!settings.contains("mode")) {
        std::cout << "ERROR: No mode found in settings.json. Should be one of \"client\" or \"host\".\n";
        return 1;
    }
    mode = settings["mode"];
    if(mode != "client" && mode != "host") {
        std::cout << "Mode invalid. Should be one of \"client\" or \"host\".\n";
        return 1;
    }

    if(mode == "client") {
        Game game{ Game::physics | Game::client };
        game.host.tryConnect(ip, 25565, 10);
        game.host.loadPacketHandler<ClientPacketHandler>(0);
        SceneId playingScene = game.loadScene<BenchmarkScene>(Scene::Flag::physics);
        std::cout << "Client starting...\n";
        SuquaLib::RunGame(game);
    } else if (mode == "host") {
        Game game{ Game::server_flags };
        game.host.loadPacketHandler<ServerPacketHandler>(0);
        std::cout << "Server starting...\n";
        SuquaLib::RunGame(game);
    }


	return 0;
}

/*
 * setup a server that just sends packets back
 * client
 * curr bench frame = tick / 5 secs (new frame every 5 secs)
 * send fram * 1KiB bytes every tick, measure rtt
 */
