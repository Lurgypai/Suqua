#include "ItemSystem.h"

#include <format>
#include <fstream>
#include <stdexcept>
#include <unordered_map>

#include "IAGunFire.h"
#include "IABasicDash.h"

#include "nlohmann/json.hpp"

using namespace nlohmann;

static void checkForField(const json& j, const std::string& badField) {
    if(!j.contains(badField)) throw std::runtime_error{
        std::format("ItemSystem: Unable to parse field \"{}\"", badField)
    };
}

void ItemSystem::loadItems(const std::string& filename) {
    std::ifstream file{filename};
    if(!file.good()) {
        throw std::runtime_error{std::format("ItemSystem: Unable to open file \"{}\"", filename)};
    }
    json fileJson = json::parse(file);

    checkForField(fileJson, "items");

    for(auto itemField : fileJson["items"]) {
        checkForField(itemField, "tag");
        std::string itemTag = itemField["tag"];

        if(!itemField.contains("ability")) {
            items.emplace(itemTag, Item{itemTag});
            continue;
        }

        std::string itemAbility = itemField["ability"];
        if(itemAbility == "ability:gun:basic") {
            checkForField(itemField, "offset");
            checkForField(itemField, "bullet_tag");
            checkForField(itemField, "chamber_size");
            checkForField(itemField, "fire_delay");
            checkForField(itemField, "reload_delay");
            checkForField(itemField, "bullet_count");
            checkForField(itemField, "bullet_spread");
            checkForField(itemField, "base_vel");
            checkForField(itemField, "vel_variance");
            items.emplace(itemTag, Item{itemTag, IAGunFire{
                itemField["offset"],
                itemField["bullet_tag"],
                itemField["chamber_size"],
                itemField["fire_delay"],
                itemField["reload_delay"],
                itemField["bullet_count"],
                itemField["bullet_spread"],
                itemField["base_vel"],
                itemField["vel_variance"]
            }});

        }
        else if(itemAbility == "ability:skill:dash") {
            items.emplace(itemTag, Item{itemTag, IABasicDash{}});
        }
    }
}

const Item& ItemSystem::getItem(const std::string& tag) const {
    auto iter = items.find(tag);
    if(iter == items.end()) throw std::runtime_error{std::format(
            "ItemSystem: Unable to find item \"{}\"",
            tag
            )};
	return items.at(tag);
}

bool ItemSystem::hasItem(const std::string& tag) const {
	return items.contains(tag);
}

const std::unordered_map<std::string, Item>& ItemSystem::getItems() const {
    return items;
}
