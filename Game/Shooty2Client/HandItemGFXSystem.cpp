#include "HandItemGFXSystem.h"

#include <fstream>

#include "nlohmann/json.hpp"

using namespace nlohmann;
static void checkForField(const json& j, const std::string& badField) {
    if(!j.contains(badField)) throw std::runtime_error{
        std::format("HandItemGFXSystem: Unable to parse field \"{}\"", badField)
    };
}

void HandItemGFXSystem::loadGfx(const ItemSystem& items) {
    for(const auto& pair : items.getItems()) {
        // split for file reading and parsing
        std::vector<std::string> tags;
        size_t start = 0;
        size_t end = pair.first.find(':', start);
        while (end != std::string::npos) {
            tags.emplace_back(pair.first.substr(start, end - start));
            start = end + 1;
            end = pair.first.find(':', start);
        }

        // get desc
        tags.emplace_back(pair.first.substr(start, pair.first.size() - start));
        std::string basePath;
        for(auto iter = tags.begin(); iter != std::prev(tags.end()); ++iter) {
            basePath += (*iter) + '/';
        }
        std::string descFilePath = basePath + tags.back() + "_desc.json";
        std::ifstream descFileStream{descFilePath};
        if(!descFileStream.good()) throw std::runtime_error{std::format(
            "InterfaceItemGFXSystem: Unable to open description {}", descFilePath
            )};

        json j = json::parse(descFileStream);

        // parse desc
        // mode
        checkForField(j, "mode");
        std::string mode = j["mode"];
        HandItemGFX::RenderMode renderMode;
        if(mode == "sprite") renderMode = HandItemGFX::sprite;
        else if(mode == "particle") renderMode = HandItemGFX::particle;
        else if(mode == "none") renderMode = HandItemGFX::none;

        // offset
        std::array<float, 2> renderOffset{};
        if(renderMode != HandItemGFX::none) {
            checkForField(j, "offset");
            renderOffset = j["offset"];
        }

        // render tag
        std::string renderTag{};
        if(renderMode == HandItemGFX::sprite) {
            std::string baseTexTag{"tex:"};
            for(auto iter = std::next(tags.begin()); iter != std::prev(tags.end()); ++iter) {
                baseTexTag += (*iter) + ':';
            }
            renderTag = baseTexTag + tags.back();
        }

        gfx.emplace(pair.first, HandItemGFX{renderTag, renderMode, {renderOffset[0], renderOffset[1]}});
    }
}

HandItemGFX HandItemGFXSystem::getGFX(const std::string& itemTag) const {
	auto item = gfx.find(itemTag);
	if (item == gfx.end()) return HandItemGFX{ "", HandItemGFX::RenderMode::none, Vec2f{} };
	return item->second;
}
