#include "EntityGenerator.h"

#include <fstream>
#include <stdexcept>

#include "nlohmann/json.hpp"

#include "EntitySpawnFunctions.h"
#include "EntityAddNetwork.h"

using namespace nlohmann;
static void checkForField(const json& j, const std::string& badField) {
    if(!j.contains(badField)) throw std::runtime_error{
        std::format("EntityGenerator: Unable to parse field \"{}\"", badField)
    };
}

static EntityAddPhysicsArgs parsePhysics(const json& addJson);
static EntityAddLivingArgs parseLiving(const json& addJson);
static EntityAddInventoryArgs parseInventory(const json& addJson);
static EntityAddHandsArgs parseHands(const json& addJson);
static EntityAddDaemonArgs parseDaemon(const json& addJson);
static EntityAddBulletArgs parseBullet(const json& addJson);
static EntityAddAIGunnerArgs parseAIGunner(const json& addJson);

EntityGenerator::EntityGenerator(const std::string& entityFile) {
    std::ifstream file{entityFile};
    if(!file.good()) throw std::runtime_error{std::format(
            "EntityGenerator: Unable to open file \"{}\"",
            entityFile
            )};
    json j = json::parse(file);
    checkForField(j, "entities");

    for(const auto& entityJson : j["entities"]) {
        checkForField(entityJson, "tag");
        std::string tag = entityJson["tag"];

        Entity entityTemplate{};

        checkForField(entityJson, "adds");
        for(const auto& addJson : entityJson["adds"]) {
            checkForField(addJson, "tag");
            std::string addTag =  addJson["tag"];

            if(addTag == "physics") {
                entityTemplate.args.emplace_back(parsePhysics(addJson));
            } else if (addTag == "living") {
                entityTemplate.args.emplace_back(parseLiving(addJson));
            } else if (addTag == "inventory") {
                entityTemplate.args.emplace_back(parseInventory(addJson));
            } else if (addTag == "hands") {
                entityTemplate.args.emplace_back(parseHands(addJson));
            } else if (addTag == "daemon") {
                entityTemplate.args.emplace_back(parseDaemon(addJson));
            } else if (addTag == "bullet") {
                entityTemplate.args.emplace_back(parseBullet(addJson));
            } else if (addTag == "ai_gunner") {
                entityTemplate.args.emplace_back(parseAIGunner(addJson));
            } else {
                throw std::runtime_error{std::format(
                        "EntityGenerator: Unable to find entity add \"{}\" while parsing",
                        addTag
                        )};
            }

            entityTemplate.adds.emplace_back(std::move(addTag));
        }
        entities.emplace(tag, std::move(entityTemplate));
    }
}

EntityId EntityGenerator::SpawnEntity(
        const std::string& tag,
        Scene& targetScene,
        const Vec2f& targetPos,
        NetworkDataComponent::Owner owner,
        const UUID& uuid) {
    auto pair = entities.find(tag);
    if(pair == entities.end()) throw std::runtime_error{std::format(
            "EntityGenerator: Unable to find entity \"{}\"",
            tag
            )};

    auto entityId = targetScene.addEntities(1)[0];
    EntityAddNetworkArgs netArgs {
        uuid,
        owner
    };
    EntityAddNetwork(entityId, netArgs);
    for(int i = 0; i != pair->second.args.size(); ++i) {
        const std::string& addTag = pair->second.adds[i];
        const auto& args = pair->second.args[i];

        if(addTag == "physics") {
            // copies
            auto physicsArgs = std::get<EntityAddPhysicsArgs>(args);
            physicsArgs.pos = targetPos;
            EntityAddPhysics(entityId, physicsArgs);
        } else if (addTag == "living") {
            EntityAddLiving(entityId, std::get<EntityAddLivingArgs>(args));
        } else if (addTag == "inventory") {
            EntityAddInventory(entityId, std::get<EntityAddInventoryArgs>(args));
        } else if (addTag == "hands") {
            EntityAddHands(entityId, std::get<EntityAddHandsArgs>(args));
        } else if (addTag == "daemon") {
            EntityAddDaemon(entityId, std::get<EntityAddDaemonArgs>(args));
        } else if (addTag == "bullet") {
            EntityAddBullet(entityId, std::get<EntityAddBulletArgs>(args));
        } else if (addTag == "ai_gunner") {
            EntityAddAIGunner(entityId, std::get<EntityAddAIGunnerArgs>(args));
        } else {
            throw std::runtime_error{std::format(
                    "EntityGenerator: Unable to find entity add \"{}\" while spawning",
                    addTag
                    )};
        }
    }

    return entityId;
}


static EntityAddPhysicsArgs parsePhysics(const json& addJson) {
    checkForField(addJson, "res");
    std::array<float, 2> res = addJson["res"];
    bool collideable = true;
    if(addJson.contains("collideable")) collideable = addJson["collideable"];
    bool collideableWith = false;
    if(addJson.contains("collideable_with")) collideableWith = addJson["collideable_with"];
    bool centered = false;
    if(addJson.contains("centered")) centered = addJson["centered"];

    return EntityAddPhysicsArgs{
        Vec2f{0.f, 0.f},
        Vec2f{res[0], res[1]},
        collideable,
        collideableWith,
        centered
    };
}
static EntityAddLivingArgs parseLiving(const json& addJson) {
    checkForField(addJson, "move_speed");
    float moveSpeed = addJson["move_speed"];
    TeamComponent::TeamId teamId = TeamComponent::TeamId::neutral;
    if(addJson.contains("team_id")) {
        std::string teamIdStr = addJson["team_id"];
        if(teamIdStr == "player") teamId = TeamComponent::TeamId::player;
        else if (teamIdStr == "enemy") teamId = TeamComponent::TeamId::enemy;
    }
    checkForField(addJson, "hurtbox_offset");
    std::array<float, 2> hurtboxOffset = addJson["hurtbox_offset"];
    checkForField(addJson, "hurtbox_res");
    std::array<float, 2> hurtboxRes = addJson["hurtbox_res"];
    checkForField(addJson, "health");
    int health = addJson["health"];
    std::string deathCallback = "default";
    if(addJson.contains("death_callback")) deathCallback = addJson["death_callback"];

    return EntityAddLivingArgs {
        moveSpeed,
        teamId,
        Vec2f{hurtboxOffset[0], hurtboxOffset[1]},
        Vec2f{hurtboxRes[0], hurtboxRes[1]},
        health,
        std::move(deathCallback)
    };
}
static EntityAddInventoryArgs parseInventory(const json& addJson) {
    std::vector<std::string> itemTags;
    std::vector<int> itemCounts;
    checkForField(addJson, "items");
    for(const auto& itemJson : addJson["items"]) {
        checkForField(itemJson, "tag");
        std::string tag = itemJson["tag"];
        int count = 1;
        if(itemJson.contains("count")) count = itemJson["count"];
        itemTags.emplace_back(std::move(tag));
        itemCounts.push_back(count);
    }
    return EntityAddInventoryArgs {
        std::move(itemTags),
        std::move(itemCounts)
    };
}
static EntityAddHandsArgs parseHands(const json& addJson) {
    checkForField(addJson, "body_offset");
    std::array<float, 2> bodyOffset = addJson["body_offset"];
    checkForField(addJson, "arm_length");
    float armLength = addJson["arm_length"];

    return EntityAddHandsArgs {
        Vec2f{bodyOffset[0], bodyOffset[1]},
        armLength
    };
}
static EntityAddDaemonArgs parseDaemon(const json& addJson) {
    checkForField(addJson, "follow_rate");
    float followRate = addJson["follow_rate"];
    checkForField(addJson, "follow_offset");
    std::array<float, 2> followOffset = addJson["follow_offset"];
    return EntityAddDaemonArgs {
        followRate,
        Vec2f{followOffset[0], followOffset[1]}
    };
}
static EntityAddBulletArgs parseBullet(const json& addJson) {
    checkForField(addJson, "res");
    std::array<float, 2> res = addJson["res"];
    TeamComponent::TeamId team = TeamComponent::TeamId::neutral;
    checkForField(addJson, "damage");
    int damage = addJson["damage"];
    checkForField(addJson, "lifetime");
    int lifetime = addJson["lifetime"];
    std::string collisionHandler = "CHKill";

    return EntityAddBulletArgs {
        Vec2f{res[0], res[1]},
        team,
        damage,
        lifetime,
        collisionHandler
    };
}
static EntityAddAIGunnerArgs parseAIGunner(const json& addJson) {
    checkForField(addJson, "follow_radius");
    float followRadius = addJson["follow_radius"];
    checkForField(addJson, "avoid_radius");
    float avoidRadius = addJson["avoid_radius"];

    return EntityAddAIGunnerArgs {
        followRadius,
        avoidRadius
    };
}

/*
void EntityGenerator::RegisterSpawnFunctions() {
    EntityGenerator::SpawnFunctions.insert(std::make_pair("entity:player:basic", SpawnPlayer));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("entity:player:daemon", SpawnDaemon));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("entity:enemy:basic", SpawnEnemy));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("entity:bullet:player:basic", SpawnBulletPlayerBasic));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("entity:bullet:enemy:basic", SpawnBulletEnemyBasic));
    EntityGenerator::SpawnFunctions.insert(std::make_pair("entity:world:teleportzone", SpawnTeleportZone));
}
*/
