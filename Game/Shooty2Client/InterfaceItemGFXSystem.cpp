#include "InterfaceItemGFXSystem.h"

#include <fstream>
#include <stdexcept>

#include "nlohmann/json.hpp"

#include "GLRenderer.h"

using namespace nlohmann;

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
        std::string smallImagePath = basePath + tags.back() + ".png";
        std::string bigImagePath = basePath + tags.back() + "_full.png";

        std::string baseTexTag{"tex:"};
        for(auto iter = std::next(tags.begin()); iter != std::prev(tags.end()); ++iter) {
            baseTexTag += (*iter) + ':';
        }

        std::string smallTexTag = baseTexTag + tags.back();
        std::string bigTexTag = baseTexTag + tags.back() + "_full";

        GLRenderer::LoadTexture(smallImagePath, smallTexTag);
        GLRenderer::LoadTexture(bigImagePath, bigTexTag);

        std::string descFilePath = basePath + tags.back() + "_desc.json";
        std::ifstream descFileStream{descFilePath};
        if(!descFileStream.good()) throw std::runtime_error{std::format(
            "InterfaceItemGFXSystem: Unable to open description {}", descFilePath
            )};

        json j = json::parse(descFileStream);

        if(!j.contains("desc")) throw std::runtime_error{
            "InterfaceItemGFXSystem: Unable to open description field \"desc\""
        };
        std::string desc = j["desc"];

        gfx.emplace(pair.first, InterfaceItemGFX{ smallTexTag, bigTexTag, desc});
    }
}

InterfaceItemGFX InterfaceItemGFXSystem::getGFX(const std::string& itemTag) const {
    auto item = gfx.find(itemTag);
    if(item == gfx.end()) return InterfaceItemGFX{ "none", "none", "Uh Oh!"};
    return item->second;
}

