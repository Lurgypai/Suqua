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
            checkForField(itemField, "bulletTag");
            checkForField(itemField, "chamberSize");
            checkForField(itemField, "fireDelay");
            checkForField(itemField, "reloadDelay");
            checkForField(itemField, "bulletCount");
            checkForField(itemField, "bulletSpread");
            checkForField(itemField, "velVariance");
            items.emplace(itemTag, Item{itemTag, IAGunFire{
                itemField["offset"],
                itemField["bulletTag"],
                itemField["chamberSize"],
                itemField["fireDelay"],
                itemField["reloadDelay"],
                itemField["bulletCount"],
                itemField["bulletSpread"],
                itemField["velVariance"]
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
