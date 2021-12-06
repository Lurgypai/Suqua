#include "ClientBoxScene.h"
#include "Game.h"

ClientBoxScene::ClientBoxScene(SceneId id_, FlagType flags_, InputDeviceId input_, const std::string& ip_, int port_) :
	BoxScene{id_, flags_},
	input{input_},
	ip{ip_},
	port{port_}
{}

void ClientBoxScene::load(Game& game)
{
	GLRenderer::LoadTexture("box.png", "box");
	GLRenderer::LoadTexture("evil_box.png", "evil_box");

	BoxScene::load(game);
	game.host.tryConnect(ip, port, 10);


	//EntitySystem::MakeComps<RenderComponent>(1, &boxId);
	//RenderComponent* render = EntitySystem::GetComp<RenderComponent>(boxId);
	//render->setDrawable<Sprite>("box");
	//render->getDrawable<Sprite>()->setScale({ 3, 3 });

	Camera cam{ {0,0}, {1920, 1080}, 1.0 };
	camId = GLRenderer::addCamera(cam);

	//addEntityInputs({ {boxId, input} });
	//connect to the server, and send a create character request
}

void ClientBoxScene::renderStep(Game& game) {
	GLRenderer::Clear();
	drawScene(game.getRender());
	GLRenderer::Swap();
}

void ClientBoxScene::linkInputToEntity(EntityId targetEntity) {
	addEntityInputs({ {targetEntity, input} });
}

//when a client connects
//tell it what its box's networkid is, and let it create its box
//tell us about everyone elses boxes
//tell everyone else that this box now exists