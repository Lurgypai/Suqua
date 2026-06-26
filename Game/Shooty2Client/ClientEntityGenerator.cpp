#include "ClientEntityGenerator.h"

#include <algorithm>
#include <fstream>
#include <stdexcept>

#include "nlohmann/json.hpp"

#include "../Shooty2Core/Shooty2Packet.h"
#include "../Shooty2Core/EntityGenerator.h"

using namespace nlohmann;
static void checkForField(const json& j, const std::string& badField) {
    if(!j.contains(badField)) throw std::runtime_error{
        std::format("ClientEntityGenerator: Unable to parse field \"{}\"", badField)
    };
}

static EntityAddCharacterGFXArgs parseCharacter(const json& addJson);
static EntityAddHandGFXArgs parseHand(const json& addJson);
static EntityAddDaemonGFXArgs parseDaemon(const json& addJson);
static EntityAddRectGFXArgs parseRect(const json& addJson);
static EntityAddTeleportGFXArgs parseTeleport(const json& addJson);

// add ai gfx component for ai states
// add character effects gfx to apply effects like on hit etc

ClientEntityGenerator::ClientEntityGenerator(const std::string& json_, Host* host_) :
    EntityGenerator{json_},
    host{host_},
    GFXFunctions{}
{
    // load entity gfx
    for(auto& pair : entities) {
        std::string path = pair.first;
        std::replace(path.begin(), path.end(), ':', '/');
        path += ".json";

        std::ifstream entityGfxFile{path};
        if(!entityGfxFile.good()) throw std::runtime_error{std::format(
                "ClientEntityGenerator: Unable to open entity gfx file \"{}\"",
                path
                )};
        json entityJson = json::parse(entityGfxFile);

        GFXEntity entityTemplate{};

        checkForField(entityJson, "adds");
        for(const auto& addJson : entityJson["adds"]) {
            checkForField(addJson, "tag");
            std::string addTag = addJson["tag"];

            if(addTag == "character") {
                entityTemplate.args.emplace_back(parseCharacter(addJson));
            } else if (addTag == "hands") {
                entityTemplate.args.emplace_back(parseHand(addJson));
            } else if (addTag == "daemon") {
                entityTemplate.args.emplace_back(parseDaemon(addJson));
            } else if (addTag == "rect") {
                entityTemplate.args.emplace_back(parseRect(addJson));
            } else if (addTag == "teleport") {
                entityTemplate.args.emplace_back(parseTeleport(addJson));
            } else {
                throw std::runtime_error{std::format(
                        "ClientEntityGenerator: GFX Add \"{}\" not found.",
                        addTag
                        )};
            }

            entityTemplate.adds.emplace_back(std::move(addTag));
        }

        entityTemplate.tag = pair.first;
        gfxEntities.emplace(pair.first, entityTemplate);
    }
}

EntityId ClientEntityGenerator::SpawnEntity(
        const std::string& tag, Scene& targetScene,
        const Vec2f& pos, NetworkDataComponent::Owner owner,
        const Suqua::UUID& uuid) {
    auto entity = EntityGenerator::SpawnEntity(tag, targetScene, pos, owner, uuid);

    //only add gfx if we need them
    auto iter = gfxEntities.find(tag);
    if(iter != gfxEntities.end()) {
        for(int i = 0; i != iter->second.adds.size(); ++i) {
            const auto& addTag = iter->second.adds[i];
            const auto& args = iter->second.args[i];
            if(addTag == "character") {
                EntityAddCharacterGFX(entity, std::get<EntityAddCharacterGFXArgs>(args));
            } else if (addTag == "hands") {
                EntityAddHandGFX(entity, std::get<EntityAddHandGFXArgs>(args));
            } else if (addTag == "daemon") {
                EntityAddDaemonGFX(entity, std::get<EntityAddDaemonGFXArgs>(args));
            } else if (addTag == "rect") {
                EntityAddRectGFX(entity, std::get<EntityAddRectGFXArgs>(args));
            } else if (addTag == "teleport") {
                EntityAddTeleportGFX(entity, std::get<EntityAddTeleportGFXArgs>(args));
            } else {
                throw std::runtime_error{std::format(
                        "ClientEntityGenerator: GFX Add \"{}\" not found while spawning.",
                        addTag
                        )};
            }
        }
    }

    if(!host->isConnected() || owner != NetworkDataComponent::Owner::local_shared) return entity;

    ByteStream spawn;
    spawn << Shooty2Packet::SpawnEntities;
    spawn << tag;
    spawn << pos;
    spawn << uuid;
    host->bufferAllDataByChannel(0, spawn);

    return entity;
}

static EntityAddCharacterGFXArgs parseCharacter(const json& addJson) {
    checkForField(addJson, "texture_path");
    std::string texPath = addJson["texture_path"];

    GLRenderer::LoadTexture(texPath, texPath);

    checkForField(addJson, "anim_path");
    std::string animJsonPath = addJson["anim_path"];
    checkForField(addJson, "offset");
    std::array<float, 2> offset = addJson["offset"];
    checkForField(addJson, "shadow_offset");
    std::array<float, 2> shadowOffset = addJson["shadow_offset"];
    bool hasUpDown = false;
    if(addJson.contains("up_down")) hasUpDown = addJson["up_down"];

    return EntityAddCharacterGFXArgs {
        texPath,
            animJsonPath,
            Vec2f{offset[0], offset[1]},
            Vec2f{shadowOffset[0], shadowOffset[1]},
            hasUpDown
    };
}
static EntityAddHandGFXArgs parseHand(const json& addJson) {
    return EntityAddHandGFXArgs{};
}
static EntityAddDaemonGFXArgs parseDaemon(const json& addJson) {
    return EntityAddDaemonGFXArgs{};
}

static EntityAddRectGFXArgs parseRect(const json& addJson) {
    checkForField(addJson, "color");
    auto& colorJson = addJson["color"];
    checkForField(colorJson, "r");
    float r = colorJson["r"];
    checkForField(colorJson, "g");
    float g = colorJson["g"];
    checkForField(colorJson, "b");
    float b = colorJson["b"];
    checkForField(colorJson, "a");
    float a = colorJson["a"];
    bool filled = true;
    if(addJson.contains("filled")) filled = addJson["filled"];
    checkForField(addJson, "res");
    std::array<float, 2> res = addJson["res"];

    return EntityAddRectGFXArgs {
        Color{r, g, b, a},
        filled,
        Vec2f{res[0], res[1]}
    };
}

static EntityAddTeleportGFXArgs parseTeleport(const json& addJson) {
    return EntityAddTeleportGFXArgs{};
}
