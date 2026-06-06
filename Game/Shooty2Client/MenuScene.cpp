#include "MenuScene.h"

#include "Game.h"
#include "MenuTab.h"

using SuperTab = MenuTab::SuperTab;
using InventoryTab = MenuTab::InventoryTab;

MenuScene::MenuScene(SceneId id_, Scene::FlagType flags_, SceneId playingScene_, InputDeviceId input_) :
    Scene{ id_, flags_ },
    playingScene{playingScene_},
    input{input_},
    superTab{ 0 },
    subTab{ 0 },
    prevSuperTab{ -1 },
    prevSubTab{ -1 },
    maxSuperTab{ 5 },
    maxSubTab{ 5 },
    superTabBox{},
    subTabBox{},
    background{},
    superTabBaseOffset{44},
    superTabOffset{105},
    subTabBaseOffset{44},
    subTabOffset{105},
    superTabNames{},
    subTabNames{},
    superTabText{},
    subTabText{}
{
    // set the y offset of tabs
    superTabBox.setPos({0, 13});
    subTabBox.setPos({0, 45});
}

inline static void updateBoxOffset(Sprite& sprite, int baseOffset, int offset, int tab) {
    auto pos = sprite.getPos();
    sprite.setPos({static_cast<float>(baseOffset + offset * tab), pos.y});
} 

void MenuScene::updateTabs() {
    if(superTab == prevSuperTab && subTab == prevSubTab) return;

    // update tab rendering
    updateBoxOffset(superTabBox, superTabBaseOffset, superTabOffset, superTab);
    updateBoxOffset(subTabBox, subTabBaseOffset, subTabOffset, subTab);

    for(int tab = 0; tab != maxSubTab; ++tab) {
        const std::string& tabName = subTabNames[superTab][tab];
        if(tabName.empty()) subTabText[tab].text = "";
        else subTabText[tab].text = tabName;
    }

    // close previous menu
    SuperTab previousTab = static_cast<SuperTab>(prevSuperTab);
    switch(previousTab) {
        case SuperTab::status:
            break;
        case SuperTab::inventory:
            closeInventoryTab();
            break;
        case SuperTab::crucible:
            break;
        case SuperTab::settings:
            break;
    }

    // open currently active menu
    SuperTab st = static_cast<SuperTab>(superTab);
    switch(st) {
        case SuperTab::status:
            break;
        case SuperTab::inventory:
            openInventoryTab(static_cast<InventoryTab>(subTab));
            break;
        case SuperTab::crucible:
            break;
        case SuperTab::settings:
            break;
        default:
            throw std::exception{};
    }
    prevSuperTab = superTab;
    prevSubTab = subTab;
}

void MenuScene::openInventoryTab(InventoryTab tab) {
    /*
     * select items in this inventory sub
     * set cur item selected to 0
     *
     */
}

void MenuScene::closeInventoryTab() {
}

void MenuScene::load(Game& game) {
	screenBuffer.bind();
	screenBuffer.addTexture2D(720, 405, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(720, 405);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	GLRenderer::LoadTexture("menu/menu.png", "menu:background");
	GLRenderer::LoadTexture("menu/superbox.png", "menu:supertab");
	GLRenderer::LoadTexture("menu/subbox.png", "menu:subtab");

    GLRenderer::LoadTexture("font/menu_title_0.png", "font:menu_title");

    superTabBox.loadTexture("menu:supertab");
    subTabBox.loadTexture("menu:subtab");
    background.loadTexture("menu:background");


    Camera cam{{0, 0}, {720, 405}, 1.f};
    camId = GLRenderer::addCamera(cam);

    superTabNames = {
        "Status",
        "Inventory",
        "Crucible",
        "Settings"
    };

    subTabNames[1] = {
        "Weapons",
        "Armor",
        "Skills",
        "Materials",
        "Other"
    };

    int textTab = 0;
    for(TextDrawable& text : superTabText) {
        text.setPos({static_cast<float>(superTabBaseOffset + superTabOffset * textTab + 9), 19});
        text.font.loadDataFile("font/menu_title.fnt");
        text.texture_tag = "font:menu_title";
        text.text = superTabNames[textTab];
        ++textTab;
    }

    textTab = 0;
    for(TextDrawable& text : subTabText) {
        text.setPos({static_cast<float>(subTabBaseOffset + subTabOffset * textTab + 9), 51});
        text.font.loadDataFile("font/menu_title.fnt");
        text.texture_tag = "font:menu_title";
        ++textTab;
    }
}

void MenuScene::physicsStep(Game& game) {
    auto cont = game.getInputDevice(input).getControllerState();
    if(cont.toggled(ControllerBits::BUTTON_5) && cont[ControllerBits::BUTTON_5]) {
        --superTab;
        if(superTab < 0) superTab = maxSuperTab;
        subTab = 0;
    }
    if(cont.toggled(ControllerBits::BUTTON_6) && cont[ControllerBits::BUTTON_6]) {
        ++superTab;
        if(superTab > maxSuperTab) superTab = 0;
        subTab = 0;
    }

    if(cont.toggled(ControllerBits::BUTTON_7) && cont[ControllerBits::BUTTON_7]) {
        --subTab;
        if(subTab < 0) subTab = maxSubTab;
    }
    if(cont.toggled(ControllerBits::BUTTON_8) && cont[ControllerBits::BUTTON_8]) {
        ++subTab;
        if(subTab > maxSubTab) subTab = 0;
    }

    updateTabs();
}

void MenuScene::renderUpdateStep(Game& game) {}

void MenuScene::renderStep(Game& game) {
    screenBuffer.bind();
	glClearColor(0.f, 0.f, 0.f, 1.f);
	GLRenderer::Clear();  
    drawScene(game.getRender());

    // draw
    background.draw();
    superTabBox.draw();
    subTabBox.draw();

    for(auto& text : superTabText) text.draw();
    for(auto& text : subTabText) text.draw();

    GLRenderer::DrawBufferedImages();

	Framebuffer::unbind();
	GLRenderer::DrawOverScreen(screenBuffer.getTexture(0).id);
}

void MenuScene::unload(Game& game) {
}

void MenuScene::onConnect(Game& game, PeerId connectingId) {
}

void MenuScene::onDisconnect(Game& game, PeerId disconnectedPeer) {
}
