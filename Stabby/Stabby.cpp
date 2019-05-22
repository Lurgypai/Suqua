// Suqua.cpp : Defines the entry point for the console application.
//

#include "pch.h"
#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>
#include <time.h>
#include <algorithm>


#include <glad\glad.h>
#include <SDL.H>
#include "stb_image.h"

#include "Shader.h"
#include "ImageGC.h"
#include "PlayerGC.h"
#include "AABB.h"
#include "Controller.h"
#include "DebugIO.h"
#include "EntitySystem.h"
#include "PhysicsAABB.h"

#include "GLRenderer.h"
#include "PlayerCam.h"

#include "ClientPlayerLC.h"
#include "ConnectCommand.h"
#include "OnlinePlayerLC.h."
#include "PlayerData.h"
#include "Stage.h"
#include "HeadParticleLC.h"

#include "ServerClientData.h"
#include "RandomUtil.h"


const int windowWidth = 1920;
const int windowHeight = 1080;

const int viewWidth = windowWidth / 3;
const int viewHeight = windowHeight / 3;

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

/*
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}
*/

int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window* window = NULL;
	window = SDL_CreateWindow("Stabby", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Window Creation Failed.");
		return 1;
	}

	GLRenderer::Init(window, { windowWidth, windowHeight }, {viewWidth, viewHeight});


	//client time
	Time_t currentTick{ 0 };

	DebugIO::startDebug("SuquaEng0.1/fonts/consolas_0.png", "SuquaEng0.1/fonts/consolas.fnt");

	PartitionID basicParticles = GLRenderer::GenParticleType(1, {"particles/basic.vert"});
	PartitionID wavyParticles = GLRenderer::GenParticleType(1, { "particles/wavy.vert" });

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(MessageCallback, 0);

	/*------------------------ Player Preparation -----------------------------*/
	Vec2f spawnPos{ 0, -32 };
	unsigned int playerId;
	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<ClientPlayerLC>(1, &playerId);
	EntitySystem::GetComp<ClientPlayerLC>(playerId)->setPos({ -2, -20 });
	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<PlayerGC>(playerId)->load("images/stabbyman.png");


	Stage stage{};
	stage.loadGraphics("images/stage.png");

	Client client{stage};
	client.setPlayer(playerId);
	DebugIO::getCommandManager().registerCommand<ConnectCommand>(ConnectCommand{ client, currentTick });

	Controller controller;

	/*---------------------------------- Camera Preparation -------------------------------------------------------*/
	PlayerCam playerCam{ playerId, viewWidth, viewHeight };
	int camId = GLRenderer::addCamera(playerCam);
	int menuCamId = GLRenderer::addCamera(Camera{ Vec2f{ 0.0f, 0.0f }, Vec2i{ windowWidth, windowHeight }, .5 });

	double gfxDelay{ 1.0 / 60 };
	Uint64 currentLog = SDL_GetPerformanceCounter();
	Uint64 currentGfx = SDL_GetPerformanceCounter();

	/*--------------------------------------------- PostProcessing -------------------------------------------------*/
	Framebuffer fb{};
	unsigned int screenTex;
	fb.bind();
	screenTex = fb.addTexture2D(viewWidth, viewHeight, GL_RGBA, GL_RGBA, NULL, GL_COLOR_ATTACHMENT0);
	fb.finalizeFramebuffer();
	Framebuffer::unbind();


	/*--------------------------------------------- GAME LOOP -----------------------------------------------*/
	bool lockFPS{ true }, lockUPS{ true };

	bool running{ true };
	while (running) {
		Uint64 now = SDL_GetPerformanceCounter();
		//logic
		Time_t elapsedTime = now - currentLog;
		bool updateLogic{ false };
		if (lockUPS)
			updateLogic = static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency() >= CLIENT_TIME_STEP;
		else
			updateLogic = true;

		if (updateLogic) {
			currentTick++;
			double ups = 1.0 / (static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency());
			DebugIO::setLine(0, "UPS: " + std::to_string(int(round(ups))));

			currentLog = now;

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
					break;
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_BACKQUOTE)
						toggleDebug = true;
					else if (e.key.keysym.sym == SDLK_BACKSPACE)
						DebugIO::backspace();
					else if (e.key.keysym.sym == SDLK_RETURN)
						DebugIO::enterInput();
					break;
				}
			}
			if (toggleDebug)
				DebugIO::toggleDebug();

			//reset / set scroll
			controller.setMouseScroll(mouseScroll);

			const Uint8 *state = SDL_GetKeyboardState(NULL);

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

			ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(playerId);
			player->update(currentTick, CLIENT_TIME_STEP, controller, stage);

			if (client.getConnected()) {
				//this needs to stay correct even if the loop isn't running. Hence, this is run based off of elapsed times.
				client.progressTime((static_cast<double>(elapsedTime) / SDL_GetPerformanceFrequency()) / GAME_TIME_STEP);

				static ControllerPacket lastSent{};

				ControllerPacket state{};
				state.time = currentTick;
				state.state = controller.getState();
				state.netId = client.getNetId();
				state.when = client.getTime();

				lastSent.when = client.getTime();
				lastSent.time = currentTick;

				if (lastSent != state) {
					//when you die you don't teleport. furthermore, there are failed predictions
					lastSent = state;
					ENetPacket * p = enet_packet_create(&state, sizeof(StatePacket), 0);
					client.send(p);

				}

				client.service(currentTick);

				if (EntitySystem::GetPool<OnlinePlayerLC>() != nullptr) {
					for (auto& onlinePlayer : *EntitySystem::GetPool<OnlinePlayerLC>()) {
						if (!onlinePlayer.isFree) {
							onlinePlayer.val.update(client.getTime());
						}
					}
				}

				static int pingDelay = 0;
				//half a second
				int pingDelayMax = 60;

				if (pingDelay != pingDelayMax) {
					pingDelay++;
				}
				else {
					pingDelay = 0;
					client.ping(currentTick);
				}



				DebugIO::setLine(3, "NetId: " + std::to_string(client.getNetId()));
				DebugIO::setLine(4, "Ping: " + std::to_string(client.getPing()));
			}

			Pool<HeadParticleLC> * heads = EntitySystem::GetPool<HeadParticleLC>();
			if (heads != nullptr) {
				for (auto& head : *heads) {
					if (!head.isFree) {
						head.val.update(CLIENT_TIME_STEP, stage);
					}
				}
			}
		}

		//gfx

		bool updateGFX{ false };
		if (lockFPS)
			updateGFX = static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency() >= gfxDelay;
		else
			updateGFX = true;
		if (updateGFX) {
			double fps = 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency());
			
			DebugIO::setLine(1, "FPS: " + std::to_string(int(round(fps))));
			//std::cout << 1.0 / (static_cast<double>(now - currentGfx) / SDL_GetPerformanceFrequency()) << std::endl;
			currentGfx = now;

			fb.bind();

			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			GLRenderer::ClearRenderBufs(GLRenderer::all);
			
			//update camera
			const Uint8 *state = SDL_GetKeyboardState(NULL);

			unsigned int debugTextBuffer = DebugIO::getRenderBuffer();
			GLRenderer::SetDefShader(ImageShader);
			GLRenderer::setCamera(camId);
			static_cast<PlayerCam &>(GLRenderer::getCamera(camId)).update(playerId);
			EntitySystem::GetComp<PlayerGC>(playerId)->update<ClientPlayerLC>();
			GLRenderer::SetBuffer(EntitySystem::GetComp<PlayerGC>(playerId)->getRenderBuffer());
			GLRenderer::Buffer(EntitySystem::GetComp<PlayerGC>(playerId)->getImgData());
			

			bool setBuffer{ true };
			Pool<OnlinePlayerLC> * onlinePlayers = EntitySystem::GetPool<OnlinePlayerLC>();
			if (onlinePlayers != nullptr) {
				for (auto& onlinePlayer : *onlinePlayers) {
					if (!onlinePlayer.isFree) {
						EntityId id = onlinePlayer.val.getId();
						PlayerGC * image = EntitySystem::GetComp<PlayerGC>(id);
						//buffering before creating images invalidates the render buffer. update may create sprites.
						image->update<OnlinePlayerLC>();
						if (setBuffer) {
							GLRenderer::SetBuffer(image->getRenderBuffer());
							setBuffer = false;
						}
						GLRenderer::Buffer(image->getImgData());
					}
				}
			}

			
			setBuffer = true;
			Pool<HeadParticleLC> * heads = EntitySystem::GetPool<HeadParticleLC>();
			if (heads != nullptr) {
				for (auto& head : *heads) {
					if (!head.isFree) {
						EntityId id = head.val.getId();
						ImageGC * image = EntitySystem::GetComp<ImageGC>(id);
						if (setBuffer) {
							GLRenderer::SetBuffer(image->getRenderBuffer());
							setBuffer = false;
						}
						image->sprite.setPos(head.val.getPos());
						GLRenderer::Buffer(image->getImgData());
					}
				}
			}

			
			GLRenderer::SetBuffer(EntitySystem::GetComp<ImageGC>(stage.getId())->getRenderBuffer());
			GLRenderer::Buffer(EntitySystem::GetComp<ImageGC>(stage.getId())->getImgData());
			
			
			GLRenderer::Draw(GLRenderer::exclude, 1, &debugTextBuffer);

			/*
			Particle p{ {0, -50}, -45.0f + (random(0, 10) - 5), 2.0f, 500, 0};
			GLRenderer::SpawnParticles(basicParticles, 1, { 0, -50 }, p);
			Particle p1{ {0, -50}, 0.0f, 0.7f, 1000, 0 };
			GLRenderer::SpawnParticles(wavyParticles, 1, { 0, -50 }, p1);

			GLRenderer::UpdateAndDrawParticles();
			*/

			Framebuffer::unbind();
			GLRenderer::SetDefShader(FullscreenShader);
			GLRenderer::bindCurrShader();
			GLRenderer::Clear(GL_COLOR_BUFFER_BIT);
			
			GLRenderer::DrawOverScreen(fb.getTexture(0).id);

			GLRenderer::setCamera(menuCamId);
			GLRenderer::SetDefShader(DebugShader);
			DebugIO::drawLines(windowHeight);

			GLRenderer::Swap();
			GLRenderer::ReadErrors();
		}
	}

	DebugIO::stopDebug();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}