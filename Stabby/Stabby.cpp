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
#include <set>

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
#include "NavMesh.h"

#include "stage.h"
#include "player.h"

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
#include "command/StaminaCommand.h"
#include "graphics/PlayerGC.h"
#include "graphics/CapturePointGC.h"
#include "graphics/HeadParticleLC.h"
#include "player/OnlinePlayerLC.h"
#include "graphics/StatBarComponent.h"

#include "nlohmann/json.hpp"

//automatic run on nvidia card if on windwos
#if defined(WIN32) || defined(_WIN32)
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
#endif

using json = nlohmann::json;
using ControllerEvent = SessionSystem::ControllerEvent;
using NetworkEvent = SessionSystem::NetworkEvent;

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

	if (!settingsFile.good()) {
		settingsFile.close();
		std::ofstream settingOut{ "settings.json" };
		if (settingOut.good()) {
			settingOut << "{\n";
			settingOut << "    \"window_width\": 1920,\n";
			settingOut << "    \"window_height\" : 1080,\n";
			settingOut << "    \"stage\" : \"stage0\",\n";
			settingOut << "    \"ip\" : \"206.189.161.67\",\n";
			settingOut << "    \"port\" : 25565,\n";
			settingOut << "    \"name\" : \"Phil\"\n";
			settingOut << "}";
			settingOut.close();
		}
		else {
			throw std::exception{};
		}
	}

	settingsFile.open("settings.json");

	json settings{};
	settingsFile >> settings;

	if (!settings.contains("window_height")) {
		settings["window_height"] = 1080;
	}
	if (!settings.contains("window_width")) {
		settings["window_width"] = 1920;
	}
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
	DebugFIO::AddFOut("plr_log.txt");
	DebugFIO::Out("plr_log.txt") << "Client Time\tNetId\tControllerState\tPrev Controller State\tPos\tVel\tState\tRoll Frame\tActive Attack\tAttack Frame\tHealth\tStun Frame\tFacing\tSpawnPoint\tAttack Freeze Frame\tFrozen\tAttack Speed\tMove Speed\tHeal Frame\tHeal Delay\tTeam Id\tStamina\tStamina Recharge Frame\tDeath Frame\tWeapon Tag\tUser Tag"; // make it output the player states as a tab separated thing


	int debugCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowWidth, windowHeight }, .5 });
	game.loadCameras(viewWidth, viewHeight);

	PartitionID blood = GLRenderer::GenParticleType("blood", 1, { "particles/blood.vert" });
	//PartitionID wavy = GLRenderer::GenParticleType("wavy", 2, { "particles/wavy.vert" });s
	//PartitionID test = GLRenderer::GenParticleType("test", 4, { "particles/test.vert" });
	PartitionID floating = GLRenderer::GenParticleType("cap_zone", 1, { "particles/cap_zone.vert" });

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
	DebugIO::getCommandManager().registerCommand<StaminaCommand>(game);
	DebugIO::getCommandManager().registerCommand<VelocityCommand>();
	DebugIO::getCommandManager().registerCommand<TeleportCommand>();
	DebugIO::getCommandManager().registerCommand<SpawnPlayerCommand>(SpawnPlayerCommand{ &game });
	DebugIO::getCommandManager().registerCommand<TeamChangeCommand>(TeamChangeCommand{ &game.client });
	DebugIO::getCommandManager().registerCommand<HealthCommand>();
	bool doFBF{ false };
	DebugIO::getCommandManager().registerCommand<FrameByFrameCommand>(doFBF);

	/*--------------------------------------------- Load more Assets --------------------------------------------------*/
	game.loadTextures();
	game.weapons.loadAttacks("attacks/hit");
	game.weapons.loadAnimations("attacks/asset");

	game.climbables.updateClimbables();

	game.startMainMenu ();

	/*--------------------------------------------- PostProcessing -------------------------------------------------*/
	Framebuffer screenBuffer{};
	unsigned int screenTex;
	screenBuffer.bind();
	screenTex = screenBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	screenBuffer.makeDepthBuffer(viewWidth, viewHeight);
	screenBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer occlusionMap{};
	unsigned int occlusionTex;
	occlusionMap.bind();
	occlusionTex = occlusionMap.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	occlusionMap.makeDepthBuffer(viewWidth, viewHeight);
	occlusionMap.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer redOutlineBuffer{};
	unsigned int redOutlineBufferTex;
	redOutlineBuffer.bind();
	redOutlineBufferTex = redOutlineBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	redOutlineBuffer.makeDepthBuffer(viewWidth, viewHeight);
	redOutlineBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer blueOutlineBuffer{};
	unsigned int blueOutlineBufferTex;
	blueOutlineBuffer.bind();
	blueOutlineBufferTex = blueOutlineBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	blueOutlineBuffer.makeDepthBuffer(viewWidth, viewHeight);
	blueOutlineBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer pingBuffer{};
	unsigned int pingTex;
	pingBuffer.bind();
	pingTex = pingBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	pingBuffer.makeDepthBuffer(viewWidth, viewHeight);
	pingBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	Framebuffer pongBuffer{};
	unsigned int pongTex;
	pongBuffer.bind();
	pongTex = pongBuffer.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	pongBuffer.makeDepthBuffer(viewWidth, viewHeight);
	pongBuffer.finalizeFramebuffer();
	Framebuffer::unbind();

	/*--------------------------------------------- GAME LOOP -----------------------------------------------*/

	unsigned int frame{};
	unsigned int updatesPerFrame = 2;

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
			game.tick++;

			if (game.isSessionGuided()) {
				game.session.tick();
				game.tick = game.session.getCurrTick();

				ControllerEvent controllerEvent = game.session.getControllerEvent();
				std::vector<NetworkEvent> netEvents = game.session.getNetworkEvents();

				if (EntitySystem::Contains<ControllerComponent>() && EntitySystem::GetComp<ControllerComponent>(game.getPlayerId()) != nullptr) {
					ControllerComponent* cont = EntitySystem::GetComp<ControllerComponent>(game.getPlayerId());
					cont->getController() = Controller{ controllerEvent.controllerState, controllerEvent.prevControllerState };
				}


			}

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
						if (client.getConnected()) {
							CommandPacket commandPacket{};
							commandPacket.command = DebugIO::getInput().substr(0, commandPacket.command.size());
							client.send<CommandPacket>(commandPacket);
						}
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

				if (!game.isSessionGuided()) {
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
					game.session.storeControllerEvent(ControllerEvent{ controller.getState(), controller.getPrevState() }, game.tick);

					constexpr bool doAi = true;
					if (doAi) {
						if (EntitySystem::Contains<AIPlayerComponent>()) {
							for (auto& ai : EntitySystem::GetPool<AIPlayerComponent>()) {
								ai.update();
							}
						}
					}
				}
				else {
					//you need to read controller events, but also read enet eventss
				}
			}

			/*------------------------------------------------------ Update States -----------------------------------------------*/

			if (canProgressFrame) {
				for (int i = 0; i != updatesPerFrame; ++i) {
					switch (game.getState())
					{
					case Game::GameState::main_menu:
						game.updateMainMenu();
						break;
					case Game::GameState::pause_menu:
						break;
					case Game::GameState::online:
						physics.runPhysics(CLIENT_TIME_STEP);
						//online players are invincible client side, so only the server applies effects
						game.combat.runAttackCheck(CLIENT_TIME_STEP);

						game.players.updateAll(CLIENT_TIME_STEP, game.getStage(), game.spawns);

						game.clientSpawner.updateAllPlayerSpawns();

						game.respawner.updateAll();

						game.clientPlayers.update(client.getTime(), game.tick);

						game.updateRespawnMenu();

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
									state.clientTime = game.tick;
									state.state = controller.getState();
									state.prevState = controller.getPrevState();
									state.when = client.getTime();
									state.netId = online->getNetId();

									//the sent state is the controller state from after the timestamped update has run
									//only compare the current buttons being pressed, not the previous state, to only send toggled controller states
									if (lastSent.state != state.state) {
										PlayerLC* player = EntitySystem::GetComp<PlayerLC>(game.getPlayerId());
										lastSent = state;

										if (player->getState().state != State::dead) {
											client.send(state);
											DebugFIO::Out("c_out.txt") << "Sent input " << static_cast<int>(state.state) << ", " << static_cast<int>(state.prevState) << " for time " << game.tick << '\n';
											//std::cout << "Sending update for time: " << lastSent.when << '\n';
										}
									}
								}
							}

							client.service();

							if (client.isBehindServer()) {
								std::cout << "We're behind the server, pinging our time and skipping a tick.\n";
								//bump the client forward
								game.tick = client.getLastServerTick();
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

							client.sendBuffered();
						}
						break;
					case Game::GameState::offline:
						physics.runPhysics(CLIENT_TIME_STEP);
						game.combat.runAttackCheck(CLIENT_TIME_STEP);
						game.players.updateAll(CLIENT_TIME_STEP, game.getStage(), game.spawns);
						game.clientSpawner.updateAllPlayerSpawns();
						game.updateRespawnMenu();
						if (EntitySystem::Contains<HeadParticleLC>()) {
							Pool<HeadParticleLC>& heads = EntitySystem::GetPool<HeadParticleLC>();
							for (auto& head : heads) {
								head.update(CLIENT_TIME_STEP);
							}
						}
						if (!game.isSessionGuided()) {
							game.mode.tickCapturePoints(game.spawns, CLIENT_TIME_STEP);
						}
						else {
							game.clientPlayers.update(client.getTime(), game.tick);
							client.readSessionEvents();

							if (client.isBehindServer()) {
								std::cout << "We're behind the server, pinging our time and skipping a tick.\n";
								//client.ping();
								client.resetBehindServer();
							}

							if (EntitySystem::Contains<OnlinePlayerLC>()) {
								for (auto& onlinePlayer : EntitySystem::GetPool<OnlinePlayerLC>()) {
									onlinePlayer.update(client.getTime());
								}
							}

							game.loadNewPlayers();
							game.loadNewCapturePoints();
						}
						break;
					case Game::GameState::stage_editor:
						game.updateEditor();
						break;
					default:
						break;
					}
					//if we aren't doing frame by frame, don't apply multiple updates
					if (!doFBF)
						break;
				}
			}
		}

		leftover = elapsedTime_;

		double ups = updates / (static_cast<double>(elapsedTime - leftover) / SDL_GetPerformanceFrequency());
		//DebugIO::setLine(0, "UPS: " + std::to_string(int(round(ups))));

		//gfx

		bool updateGFX{ false };
		if (!doFBF) {
			if (lockFPS)
				updateGFX = static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency() >= gfxDelay;
			else
				updateGFX = true;
		}
		else {
			updateGFX = canProgressFrame;
		}
		if (updateGFX) {
			frame++;
			double fps = 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency());

			//DebugIO::setLine(1, "FPS: " + std::to_string(int(round(fps))));
			//std::cout << 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency()) << std::endl;
			currentGfx = now;

			//draw everything to the framebuffer
			pingBuffer.bind();

			GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//update camera
			const Uint8* state = SDL_GetKeyboardState(NULL);

			switch (game.getState()) {
			case Game::GameState::offline:
			case Game::GameState::online:
			{
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
				game.updateWeaponMenu();
				game.updateInGameUI();
				break;
			}
			case Game::GameState::stage_editor:
				game.updateEditorCamera();
				game.editables.updateGfx();
				break;
			case Game::GameState::main_menu:
				break;
			case Game::GameState::pause_menu:
				break;
			}



			game.renderAll(gfxDelay);

			/*
			GLRenderer::setCamera(1);
			if (EntitySystem::Contains<AIPlayerComponent>()) {
				for (auto& aiPlayer : EntitySystem::GetPool<AIPlayerComponent>()) {
					for (auto& id : aiPlayer.getCurrentPath()) {
						const ClimbableNavMesh& mesh = AIPlayerComponent::getNavMesh();
						const auto& collider = mesh.getZone(id).area;
						Primitive p{ {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, collider.pos + collider.res, {collider.pos.x, collider.pos.y + collider.res.y}}, -1.0, Color{0.0, 1.0, 1.0, 1.0} };
						GLRenderer::DrawPrimitive(p);
					}

					auto collider = aiPlayer.getCurrZone().area;
					Primitive p{ {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, collider.pos + collider.res, {collider.pos.x, collider.pos.y + collider.res.y}}, -1.0, Color{1.0, 0.5, 1.0, 1.0} };
					GLRenderer::DrawPrimitive(p);

					collider = aiPlayer.getTargetZone().area;
					p = { {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, collider.pos + collider.res, {collider.pos.x, collider.pos.y + collider.res.y}}, -1.0, Color{1.0, 0.5, 0.0, 1.0} };
					GLRenderer::DrawPrimitive(p);

				}
			}
			*/

			//pathfinding rendering
			
			/*
			GLRenderer::setCamera(1);
			if (EntitySystem::Contains<PhysicsComponent>()) {
				/*
				static bool meshGenerated = false;
				static ClimbableNavMesh n{20, 4, 20};
				if (!meshGenerated) {
					n.addPhysicsMesh();
					n.addClimbableMesh();
					meshGenerated = true;
				}
				
				const ClimbableNavMesh& n = AIPlayerComponent::getNavMesh();

				std::set<std::pair<unsigned int, unsigned int>> drawnPaths;
				for (auto& pair : n.getZones()) {
					const AABB& collider = pair.second.area;
					Color c{ 0.5, 0.5, 0.5, 1.0 };
					if (pair.second.typeTag == "walk") {
						c = { 1.0, 0, 0.8, 0.5 };
					}
					else if (pair.second.typeTag == "climb") {
						c = { 1.0, 1.0, 0.0, 0.5 };

					}
					Primitive p{ {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, collider.pos + collider.res, {collider.pos.x, collider.pos.y + collider.res.y}}, -1.0, c };
					GLRenderer::DrawPrimitive(p);

					/*
					for (auto& adjacentId : pair.second.adjacentZones) {
						if (drawnPaths.find(std::pair<unsigned int, unsigned int>{adjacentId, pair.first}) == drawnPaths.end()) {
							auto& zone = n.getZone(adjacentId);

							Primitive p{ {pair.second.area.center() + Vec2f{-5, 0}, zone.area.center() + Vec2f { -5, 0 }}, -1.0, Color{0.0, 1.0, 1.0, 1.0} };
							GLRenderer::DrawPrimitive(p);
						}
						else {
							auto& zone = n.getZone(adjacentId);

							Primitive p{ {pair.second.area.center() + Vec2f{5, 0}, zone.area.center() + Vec2f{5, 0}}, -1.0, Color{1.0, 0.5, 0.0, 1.0} };
							GLRenderer::DrawPrimitive(p);
						}
						drawnPaths.insert(std::pair<unsigned int, unsigned int>{pair.first, adjacentId});
					}
					
				}
				*/

				/*
				PhysicsComponent* playerPhysics = EntitySystem::GetComp<PhysicsComponent>(game.getPlayerId());
				const auto& startZone = n.getZone(playerPhysics->getPos() + Vec2f{0, -1});
				const auto& endZone = n.getZone({ 0, 0 });
				auto path = n.getPath(startZone->id, endZone->id);
				for (auto& id : path) {
					NavZone z = n.getZone(id);
					const AABB& collider = z.area;
					Primitive p{ {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, collider.pos + collider.res, {collider.pos.x, collider.pos.y + collider.res.y}}, -1.0, Color{0.5, 0.0, 1.0, 1.0} };
					GLRenderer::DrawPrimitive(p);
				}
				PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(game.getPlayerId());
				const auto& zone = n.getZone(physics->getPos() + Vec2f{0, -1});
				const auto& collider = zone->area;
				Primitive p{ {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, collider.pos + collider.res, {collider.pos.x, collider.pos.y + collider.res.y}}, -1.0, Color{1.0, 0.5, 0.0, 1.0} };
				GLRenderer::DrawPrimitive(p);
				DebugIO::setLine(8, "left: " + std::to_string(zone->leftBlocked));
				DebugIO::setLine(9, "right: " + std::to_string(zone->rightBlocked));

				for (const auto& adjacentId : zone->adjacentZones) {
					auto& adjZone = n.getZone(adjacentId);

					Primitive p{ {collider.center(), adjZone.area.center()}, -1.0, Color{0.0, 1.0, 1.0, 1.0} };
					GLRenderer::DrawPrimitive(p);
				}
			}
			*/
			

			//hitbox rendering
			/*
			GLRenderer::setCamera(1);
			if (EntitySystem::Contains<CombatComponent>()) {
				int i = 0;
				for (auto& combat : EntitySystem::GetPool<CombatComponent>()) {
					if (combat.getActiveHitbox() != nullptr) {
						AABB box = combat.getActiveHitbox()->hit;
						Primitive p{ {box.pos, {box.pos.x + box.res.x, box.pos.y}, box.pos + box.res, {box.pos.x, box.pos.y + box.res.y}}, -1.0, Color{0.0, 1.0, 0.5, 1.0} };
						GLRenderer::DrawPrimitive(p);
					}
					AABB hurtBox = combat.getBoundingBox();
					Primitive p{ {hurtBox.pos, {hurtBox.pos.x + hurtBox.res.x, hurtBox.pos.y}, hurtBox.pos + hurtBox.res, {hurtBox.pos.x, hurtBox.pos.y + hurtBox.res.y}}, -1.0, Color{0.8, 0.0, 0.5, 1.0} };
					GLRenderer::DrawPrimitive(p);
				}
			}
			*/
			
			
			
			//Draw all the physics components to the occlusion map.
			occlusionMap.bind();
			GLRenderer::setCamera(1);
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			if (EntitySystem::Contains<PhysicsComponent>()) {
				for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
					if (physics.collideable) {
						const auto& collider = physics.getCollider();
						Primitive p{ {collider.pos, {collider.pos.x + collider.res.x, collider.pos.y}, {collider.pos.x, collider.pos.y + collider.res.y}, collider.pos + collider.res}, -1.0, Color{1.0, 1.0, 1.0, 1.0} };
						GLRenderer::DrawFilledPrimitive(p);
					}
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
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			blueOutlineBuffer.bind();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			GLRenderer::DrawOverScreen(pingBuffer.getTexture(0).id);
			GLRenderer::setCamera(debugCamId);
			DebugIO::drawLines();

			GLRenderer::Swap();
			GLRenderer::ReadErrors();

			//after drawing clean up dead entities. This way everything gets a chance to be drawn.
			EntitySystem::FreeDeadEntities();
		}

		if (doFBF)
			canProgressFrame = false;
	}

	game.session.serialize("session");

	DebugIO::stopDebug();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

/*
to do
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

//load and store the buttons
//clear the main menu
//load weapon menu into menu camera
//add control for displaying it

//packet times are desyncing ;D