#include "MenuScene.h"

#include "Game.h"

MenuScene::MenuScene(SceneId id_, Scene::FlagType flags_, SceneId playingScene_, InputDeviceId input_) :
    Scene{ id_, flags_ },
    playingScene{playingScene_},
    input{input_},
    superTab{ 0 },
    subTab{ 0 },
    maxSuperTab{ 3 },
    maxSubTab{ 3 },
    superTabBox{},
    subTabBox{},
    background{},
    superTabBaseOffset{44},
    superTabOffset{105},
    subTabBaseOffset{44},
    subTabOffset{105}
{
    // set the y offset of tabs
    superTabBox.setPos({0, 13});
    subTabBox.setPos({0, 45});
}

void updateBoxOffset(Sprite& sprite, int baseOffset, int offset, int tab) {
    auto pos = sprite.getPos();
    sprite.setPos({static_cast<float>(baseOffset + offset * tab), pos.y});
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

    superTabBox.loadTexture("menu:supertab");
    subTabBox.loadTexture("menu:subtab");
    background.loadTexture("menu:background");

    updateBoxOffset(superTabBox, superTabBaseOffset, superTabOffset, superTab);
    updateBoxOffset(subTabBox, subTabBaseOffset, subTabOffset, subTab);
    Camera cam{{0, 0}, {720, 405}, 1.f};
    camId = GLRenderer::addCamera(cam);
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
}

void MenuScene::renderUpdateStep(Game& game) {
    updateBoxOffset(superTabBox, superTabBaseOffset, superTabOffset, superTab);
    updateBoxOffset(subTabBox, subTabBaseOffset, subTabOffset, subTab);
}

void MenuScene::renderStep(Game& game) {
    screenBuffer.bind();
	glClearColor(0.f, 0.f, 0.f, 1.f);
	GLRenderer::Clear();  
    drawScene(game.getRender());

    // draw
    background.draw();
    superTabBox.draw();
    subTabBox.draw();
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
