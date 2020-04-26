// Suqua.cpp : Defines the entry point for the console application.
//

#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>
#include <time.h>
#include <algorithm>

#include <glad/glad.h>
#include <SDL.h>
#include "stb_image.h"
#include "Shader.h"
#include "AABB.h"
#include "Controller.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "GLRenderer.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "RandomUtil.h"
#include "DebugFIO.h"
#include <ControllerComponent.h>
#include "RectDrawable.h"
#include "TextDrawable.h"

#include "stage.h"

#include "game/Game.h"
#include "graphics/camera/PlayerCam.h"
#include "graphics/camera/EditorCam.h"
#include "command/StartCommand.h"
#include "command/AttackSpeedCommand.h"
#include "command/MoveSpeedCommand.h"
#include "command/SaveStageCommand.h"
#include "command/LoadStageCommand.h"
#include "command/KillCommand.h"
#include "command/WeaponCommand.h"
#include "command/VelocityCommand.h"
#include "command/TeleportCommand.h"
#include "command/SpawnPlayerCommand.h"
#include "command/TeamChangeCommand.h"
#include "command/HealthCommand.h"
#include "command/FrameByFrameCommand.h"
#include "graphics/PlayerGC.h"
#include "graphics/CapturePointGC.h"
#include "graphics/HeadParticleLC.h"
#include "player/OnlinePlayerLC.h"

#include "nlohmann/json.hpp"

//automatic run on nvidia card if on windwos
#if defined(WIN32) || defined(_WIN32)
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

using json = nlohmann::json;

const int viewWidth = 640;
const int viewHeight = 360;

void MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}


int main(int argc, char* argv[]) {

	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	const int windowWidth = settings["window_width"];
	const int windowHeight = settings["window_height"];

	Game game{};

	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = NULL;
	window = SDL_CreateWindow("Stabby", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Failed.");
		return 1;
	}

	GLRenderer::Init(window, { windowWidth, windowHeight }, { viewWidth, viewHeight });

	DebugIO::startDebug("suqua/fonts/consolas_0.png", "suqua/fonts/consolas.fnt");
	DebugFIO::AddFOut("c_out.txt");

	int debugCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowWidth, windowHeight }, .5 });
	game.loadCameras(viewWidth, viewHeight);

	PartitionID blood = GLRenderer::GenParticleType("blood", 1, { "particles/blood.vert" });
	PartitionID wavy = GLRenderer::GenParticleType("wavy", 1, { "particles/wavy.vert" });
	PartitionID test = GLRenderer::GenParticleType("test", 4, { "particles/test.vert" });
	PartitionID floating = GLRenderer::GenParticleType("float", 1, { "particles/float.vert" });

	int colorShader;
	GLRenderer::LoadShaders({ {"shaders/color.vert", "shaders/color.frag"} }, &colorShader);
	GLRenderer::GetShaderRef(colorShader).uniform2f("viewRes", viewWidth, viewHeight);
	int outlineShader;
	GLRenderer::LoadShaders({ {"shaders/outline.vert", "shaders/outline.frag"} }, &outlineShader);
	GLRenderer::GetShaderRef(outlineShader).uniform2f("viewRes", viewWidth, viewHeight);
	Sprite colors{ "images/palettes/test.png" };

	glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(MessageCallback, 0);

	PhysicsSystem& physics = game.physics;
	Client& client = game.client;

	/*--------------------------------------------- Commands ---------------------------------------------------------*/
	DebugIO::getCommandManager().registerCommand<StartCommand>(StartCommand{ game });
	DebugIO::getCommandManager().registerCommand<AttackSpeedCommand>(AttackSpeedCommand{ game });
	DebugIO::getCommandManager().registerCommand<MoveSpeedCommand>(MoveSpeedCommand{ game });
	DebugIO::getCommandManager().registerCommand<SaveStageCommand>(SaveStageCommand{ game.editables });
	DebugIO::getCommandManager().registerCommand<LoadStageCommand>(LoadStageCommand{ game.editables });
	//DebugIO::getCommandManager().registerCommand<SpawnZombieCommand>();
	DebugIO::getCommandManager().registerCommand<KillCommand>();
	DebugIO::getCommandManager().registerCommand<WeaponCommand>(game);
	DebugIO::getCommandManager().registerCommand<VelocityCommand>();
	DebugIO::getCommandManager().registerCommand<TeleportCommand>();
	DebugIO::getCommandManager().registerCommand<SpawnPlayerCommand>(SpawnPlayerCommand{ &game.players, &game.weapons });
	DebugIO::getCommandManager().registerCommand<TeamChangeCommand>(TeamChangeCommand{ &game.client });
	DebugIO::getCommandManager().registerCommand<HealthCommand>();
	bool doFBF{ false };
	DebugIO::getCommandManager().registerCommand<FrameByFrameCommand>(doFBF);

	/*--------------------------------------------- Load more Assets --------------------------------------------------*/
	game.loadTextures();
	game.weapons.loadAttacks("attacks/hit");
	game.weapons.loadAnimations("attacks/asset");

	game.climbables.updateClimbables();

	game.startMainMenu();

	/*--------------------------------------------- PostProcessing -------------------------------------------------*/
	Framebuffer screenBuffer{};
	unsigned int screenTex;
	screenBuffer.bind();
	screenTex = screenBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer occlusionMap{};
	unsigned int occlusionTex;
	occlusionMap.bind();
	occlusionTex = occlusionMap.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	occlusionMap.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer redOutlineBuffer{};
	unsigned int redOutlineBufferTex;
	redOutlineBuffer.bind();
	redOutlineBufferTex = redOutlineBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	redOutlineBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer blueOutlineBuffer{};
	unsigned int blueOutlineBufferTex;
	blueOutlineBuffer.bind();
	blueOutlineBufferTex = blueOutlineBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	blueOutlineBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer pingBuffer{};
	unsigned int pingTex;
	pingBuffer.bind();
	pingTex = pingBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	pingBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer pongBuffer{};
	unsigned int pongTex;
	pongBuffer.bind();
	pongTex = pongBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	pongBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	/*--------------------------------------------- GAME LOOP -----------------------------------------------*/

	unsigned int frame{};
	unsigned int tick{};

	double gfxDelay{ 1.0 / 60 };
	Uint64 currentLog = SDL_GetPerformanceCounter();
	Uint64 currentGfx = SDL_GetPerformanceCounter();
	Uint64 leftover{ 0 };

	bool lockFPS{ true };

	bool canProgressFrame = true;

	bool running{ true };
	while (running) {
		Uint64 now = SDL_GetPerformanceCounter();
		//logic
		Time_t elapsedTime = now - currentLog;
		currentLog = now;
		Time_t delta = CLIENT_TIME_STEP * SDL_GetPerformanceFrequency();

		double updates{ 0.0 };
		Time_t elapsedTime_ = elapsedTime + leftover;
		for (; elapsedTime_ >= delta; elapsedTime_ -= delta) {
			++updates;
			tick++;
			game.tick++;
			client.clientTime++;

			/*--------------------------- Handle Input ----------------------------*/

			int mouseScroll{ 0 };
			//wait till after polling events to toggle debug, so we don't output the opening char
			bool toggleDebug{ false };
			SDL_Event e;
			while (SDL_PollEvent(&e)) {
				switch (e.type) {
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_CLOSE)
						running = false;
					break;
				case SDL_MOUSEWHEEL:
					if (e.wheel.y > 0)
						++mouseScroll;
					else if (e.wheel.y < 0)
						--mouseScroll;
					break;
				case SDL_TEXTINPUT:
					if (DebugIO::getOpen())
						DebugIO::addInput(e.text.text);
					game.menus.inputAllMenus(e.text.text);
					break;
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_BACKQUOTE)
						toggleDebug = true;
					else if (e.key.keysym.sym == SDLK_BACKSPACE) {
						game.menus.backspaceAllMenus();
						DebugIO::backspace();
					}
					else if (e.key.keysym.sym == SDLK_RETURN) {
						DebugIO::enterInput();
						game.menus.enterTextAllMenus();
					}
					else if (e.key.keysym.sym == SDLK_l)
						canProgressFrame = true;
					else if (e.key.keysym.sym == SDLK_k) {
						canProgressFrame = true;
						doFBF = false;
					}
					break;
				}
			}
			if (toggleDebug)
				DebugIO::toggleDebug();

			if (EntitySystem::Contains<ControllerComponent>() && EntitySystem::GetComp<ControllerComponent>(game.getPlayerId()) != nullptr) {
				ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(game.getPlayerId());
				auto& controller = cont->getController();
				//reset / set scroll
				controller.setMouseScroll(mouseScroll);

				const Uint8* state = SDL_GetKeyboardState(NULL);

				if (!DebugIO::getOpen()) {
					//update controllers

					controller.set(ControllerBits::UP, state[SDL_SCANCODE_UP]);
					controller.set(ControllerBits::DOWN, state[SDL_SCANCODE_DOWN]);
					controller.set(ControllerBits::LEFT, state[SDL_SCANCODE_LEFT]);
					controller.set(ControllerBits::RIGHT, state[SDL_SCANCODE_RIGHT]);
					controller.set(ControllerBits::BUTTON_1, state[SDL_SCANCODE_Z]);
					controller.set(ControllerBits::BUTTON_2, state[SDL_SCANCODE_X]);
					controller.set(ControllerBits::BUTTON_3, state[SDL_SCANCODE_C]);
				}
				else {
					controller.off(ControllerBits::ALL);
				}
				constexpr bool aiDo{ false };
				if (aiDo) {
					for (auto& aiCont : EntitySystem::GetPool<ControllerComponent>()) {
						if (aiCont.getId() != game.getPlayerId()) {
							auto& aiController = aiCont.getController();
							aiController.set(ControllerBits::UP, rand() % 2);
							aiController.set(ControllerBits::DOWN, rand() % 2);
							aiController.set(ControllerBits::LEFT, rand() % 2);
							aiController.set(ControllerBits::RIGHT, rand() % 2);
							aiController.set(ControllerBits::BUTTON_1, rand() % 2);
							aiController.set(ControllerBits::BUTTON_2, rand() % 2);
							aiController.set(ControllerBits::BUTTON_3, rand() % 2);
						}
					}
				}
			}

			/*------------------------------------------------------ Update States -----------------------------------------------*/

			if (canProgressFrame) {
				switch (game.getState())
				{
				case GameState::main_menu:
					game.updateMainMenu();
					break;
				case GameState::pause_menu:
					break;
				case GameState::online:
					game.players.updateAll(CLIENT_TIME_STEP, game.getStage(), game.spawns);
					physics.runPhysics(CLIENT_TIME_STEP);
					game.combat.runAttackCheck(CLIENT_TIME_STEP);

					game.clientPlayers.update(client.getTime(), client.clientTime);

					if (client.getConnected()) {
						OnlineComponent* online = EntitySystem::GetComp<OnlineComponent>(game.getPlayerId());


						//this needs to stay correct even if the loop isn't running. Hence, this is run based off of elapsed times.
						client.progressTime((static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency()) / GAME_TIME_STEP);

						if (EntitySystem::Contains<ControllerComponent>()) {
							ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(game.getPlayerId());
							if (cont != nullptr) {
								auto& controller = cont->getController();
								static ControllerPacket lastSent{};

								ControllerPacket state{};
								state.clientTime = client.clientTime;
								state.state = controller.getState();
								state.when = client.getTime();
								state.netId = online->getNetId();

								lastSent.when = client.getTime();
								lastSent.clientTime = client.clientTime;

								//the sent state is the controller state from after the timestamped update has run
								if (lastSent != state) {
									lastSent = state;
									client.send(state);
									DebugFIO::Out("c_out.txt") << "Sent input " << static_cast<int>(state.state) << " for time " << client.clientTime << '\n';
									//std::cout << "Sending update for time: " << lastSent.when << '\n';
								}
							}
						}

						client.service();

						if (client.isBehindServer()) {
							std::cout << "We're behind the server, pinging our time.\n";
							client.ping();
							client.resetBehindServer();
						}

						if (EntitySystem::Contains<OnlinePlayerLC>()) {
							for (auto& onlinePlayer : EntitySystem::GetPool<OnlinePlayerLC>()) {
								onlinePlayer.update(client.getTime());
							}
						}

						game.loadNewPlayers();
						game.loadNewCapturePoints();

						DebugIO::setLine(3, "NetId: " + std::to_string(online->getNetId()));
						DebugIO::setLine(4, "Ping: " + std::to_string(client.getPing()));
					}
					break;
				case GameState::offline:
					game.players.updateAll(CLIENT_TIME_STEP, game.getStage(), game.spawns);
					physics.runPhysics(CLIENT_TIME_STEP);
					game.combat.runAttackCheck(CLIENT_TIME_STEP);
					if (EntitySystem::Contains<HeadParticleLC>()) {
						Pool<HeadParticleLC>& heads = EntitySystem::GetPool<HeadParticleLC>();
						for (auto& head : heads) {
							head.update(CLIENT_TIME_STEP);
						}
					}

					game.mode.tickCapturePoints(game.spawns, CLIENT_TIME_STEP);
					break;
				case GameState::stage_editor:
					game.updateEditor();
					break;
				default:
					break;
				}
				canProgressFrame = true;
			}
		}

		leftover = elapsedTime_;

		double ups = updates / ((static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency()));
		DebugIO::setLine(0, "UPS: " + std::to_string(int(round(ups))));

		//gfx

		bool updateGFX{ false };
		if (!doFBF) {
			if (lockFPS)
				updateGFX = static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency() >= gfxDelay;
			else
				updateGFX = true;
		}
		else {
			updateGFX = (tick % static_cast<int>(gfxDelay / CLIENT_TIME_STEP) == 0);
		}
		if (updateGFX) {
			frame++;
			double fps = 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency());

			DebugIO::setLine(1, "FPS: " + std::to_string(int(round(fps))));
			//std::cout << 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency()) << std::endl;
			currentGfx = now;

			//draw everything to the framebuffer
			pingBuffer.bind();

			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);

			//update camera
			const Uint8* state = SDL_GetKeyboardState(NULL);

			switch (game.getState()) {
			case GameState::offline:
			case GameState::online:
				//players
				if (EntitySystem::Contains<PlayerGC>()) {
					for (auto& comp : EntitySystem::GetPool<PlayerGC>()) {
						comp.updateState(gfxDelay);
					}
				}

				//capture points
				if (EntitySystem::Contains<CapturePointGC>()) {
					for (auto& comp : EntitySystem::GetPool<CapturePointGC>()) {
						comp.update(gfxDelay);
					}
				}

				game.updatePlayerCamera();
				break;
			case GameState::stage_editor:
				game.updateEditorCamera();
				game.editables.updateGfx();
				break;
			case GameState::main_menu:
				break;
			case GameState::pause_menu:
				break;
			}

			game.renderAll(gfxDelay);

			//hitbox rendering
			/*
			static bool loadedDebugBoxes{ false };
			EntityId debugBoxes[20];
			if (!loadedDebugBoxes) {
				EntitySystem::GenEntities(20, debugBoxes);
				EntitySystem::MakeComps<RenderComponent>(20, debugBoxes);
				for (int i = 0; i != 20; ++i) {
					EntitySystem::GetComp<RenderComponent>(debugBoxes[i])->loadDrawable<RectDrawable>();
				}
				loadedDebugBoxes = true;
			}
			if (EntitySystem::Contains<CombatComponent>()) {
				int i = 0;
				for (auto& combat : EntitySystem::GetPool<CombatComponent>()) {
					RenderComponent* hitboxRender = EntitySystem::GetComp<RenderComponent>(debugBoxes[i++]);
					if (hitboxRender == nullptr) {
						loadedDebugBoxes = false;
						break;
					}
					if (combat.getActiveHitbox() != nullptr) {
						AABB box = combat.getActiveHitbox()->hit;
						EntityId hitbox = hitboxRender->getId();
						PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(hitbox);
						pos->pos = box.pos;
						hitboxRender->getDrawable<RectDrawable>()->shape = box;
						hitboxRender->getDrawable<RectDrawable>()->r = 1.0;
						hitboxRender->getDrawable<RectDrawable>()->g = 0.0;
						hitboxRender->getDrawable<RectDrawable>()->b = 0.0;
					}
					RenderComponent* hurtboxRender = EntitySystem::GetComp<RenderComponent>(debugBoxes[i++]);
					AABB hurtBox = combat.getBoundingBox();
					PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(hurtboxRender->getId());
					pos->pos = hurtBox.pos;
					hurtboxRender->getDrawable<RectDrawable>()->shape = hurtBox;
					hurtboxRender->getDrawable<RectDrawable>()->r = 0.0;
					hurtboxRender->getDrawable<RectDrawable>()->g = 0.0;
					hurtboxRender->getDrawable<RectDrawable>()->b = 1.0;
				}
			}
			*/
			
			//Draw all the physics components to the occlusion map.
			occlusionMap.bind();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			if (EntitySystem::Contains<PhysicsComponent>()) {
				for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
					if(physics.collideable)
						GLRenderer::DrawFilledPrimitives({ physics.getCollider() }, 1.0, 1.0, 1.0);
				}
			}
			

			//draw the pixels to the framebuffer, draw the framebuffer over the screen

			pingBuffer.bind();

			//particles
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, occlusionMap.getTexture(0).id);
			const auto& playerCam = game.getPlayerCam();
			GLRenderer::getComputeShader("blood").use();
			GLRenderer::getComputeShader("blood").uniform2f("camPos", playerCam.pos.x, playerCam.pos.y);
			GLRenderer::getComputeShader("blood").uniform1f("zoom", playerCam.camScale);
			GLRenderer::setCamera(1); //hackish fix, player camera
			GLRenderer::UpdateAndDrawParticles();
			
			redOutlineBuffer.bind();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);

			blueOutlineBuffer.bind();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);

			if (EntitySystem::Contains<PlayerStateComponent>()) {
				for (auto& player : EntitySystem::GetPool<PlayerStateComponent>()) {
					CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(player.getId());
					RenderComponent* render = EntitySystem::GetComp<RenderComponent>(player.getId());
					if (combat->teamId == 1) {
						redOutlineBuffer.bind();
						game.render.draw(*render);
					}
					else if (combat->teamId == 2) {
						blueOutlineBuffer.bind();
						game.render.draw(*render);
					}
				}
			}
			

			
			pingBuffer.bind();
			GLRenderer::GetShaderRef(outlineShader).use();
			GLRenderer::GetShaderRef(outlineShader).uniform3f("color", 1.0, 0.0, 0.0);
			GLRenderer::DrawOverScreen(redOutlineBuffer.getTexture(0).id, viewWidth, viewHeight);
			GLRenderer::GetShaderRef(outlineShader).uniform3f("color", 0.0, 0.0, 1.0);
			GLRenderer::DrawOverScreen(blueOutlineBuffer.getTexture(0).id, viewWidth, viewHeight);
			

			//palettes, for later
			//pongBuffer.bind();
			//glActiveTexture(GL_TEXTURE1);
			//glBindTexture(GL_TEXTURE_2D, game.palettes.getCurrentPalette().getTexture());
			//GLRenderer::GetShaderRef(colorShader).use();
			//GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			//GLRenderer::DrawOverScreen(pingBuffer.getTexture(0).id, viewWidth, viewHeight);
			
			//draw the debugio over the screen

			//finished ping-ponging, draw to screen

			Framebuffer::unbind();
			GLRenderer::SetDefShader(FullscreenShader);
			GLRenderer::bindCurrShader();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);

			GLRenderer::DrawOverScreen(pingBuffer.getTexture(0).id);

			GLRenderer::setCamera(debugCamId);
			DebugIO::drawLines();

			GLRenderer::Swap();
			//GLRenderer::ReadErrors();

			//after drawing clean up dead entities. This way everything gets a chance to be drawn.
			EntitySystem::FreeDeadEntities();
		}

		if (doFBF)
			canProgressFrame = false;
	}

	DebugIO::stopDebug();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

/*
to do
CString - c string wrapper with std::string auto conversion and better interface (=, ==, etc)
player weapon syncing
	- all attacks loaded on game startup.
	- all combat components have their own hashmap of attacks (means they need to be created by the CombatSystem, and their attackMap set by it)
	- currAttack pointer, set by "setAttack(const std::string& attack_tag)"
weapon ui
	tree structure for autoname population
	grid topped by search bar
	grid populates as search bar is filled
when a weapon is selected online ->
	request a weapon change
	if the state change succeeds, your weapon tag will be out of date, and will get updated by onlinePlayerLC

	requirements for weapon change:
		dead.

-how to puppet dominationmode?
ui
weapon selector
capture point particles
game state title msgs
MAYBE
player msgs
*/