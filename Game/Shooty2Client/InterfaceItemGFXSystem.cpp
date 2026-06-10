#include "InterfaceItemGFXSystem.h"

#include <fstream>
#include <stdexcept>

#include "nlohmann/json.hpp"

#include "GLRenderer.h"

using namespace nlohmann;
static void checkForField(const json& j, const std::string& badField) {
    if(!j.contains(badField)) throw std::runtime_error{
        std::format("InterfaceItemGFXSystem: Unable to parse field \"{}\"", badField)
    };
}

void InterfaceItemGFXSystem::loadGfx(const ItemSystem& items) {
    for(const auto& pair : items.getItems()) {
        std::vector<std::string> tags;
        size_t start = 0;
        size_t end = pair.first.find(':', start);
        while (end != std::string::npos) {
            tags.emplace_back(pair.first.substr(start, end - start));
            start = end + 1;
            end = pair.first.find(':', start);
        }
        tags.emplace_back(pair.first.substr(start, pair.first.size() - start));
        std::string basePath;
        for(auto iter = tags.begin(); iter != std::prev(tags.end()); ++iter) {
            basePath += (*iter) + '/';
        }
        std::string descFilePath = basePath + tags.back() + "_desc.json";
        std::string baseTexTag{"tex:"};
        for(auto iter = std::next(tags.begin()); iter != std::prev(tags.end()); ++iter) {
            baseTexTag += (*iter) + ':';
        }

        std::ifstream descFileStream{descFilePath};
        if(!descFileStream.good()) throw std::runtime_error{std::format(
            "InterfaceItemGFXSystem: Unable to open description {}", descFilePath
            )};

        json j = json::parse(descFileStream);
        std::string smallTexTag = "none";
        if(j.contains("ui_small")) {
            smallTexTag = baseTexTag + tags.back() + "_ui_small";
            GLRenderer::LoadTexture(j["ui_small"], smallTexTag);
        }
        std::string bigTexTag = "none";
        if(j.contains("ui_big")) {
            bigTexTag = baseTexTag + tags.back() + "_ui_big";
            GLRenderer::LoadTexture(j["ui_big"], bigTexTag);
        }

        std::string desc = "Uh Oh!";
        if(j.contains("desc")) desc = j["desc"];

        gfx.emplace(pair.first, InterfaceItemGFX{ smallTexTag, bigTexTag, desc});
    }
}

InterfaceItemGFX InterfaceItemGFXSystem::getGFX(const std::string& itemTag) const {
    auto item = gfx.find(itemTag);
    if(item == gfx.end()) return InterfaceItemGFX{ "none", "none", "Uh Oh!"};
    return item->second;
}

