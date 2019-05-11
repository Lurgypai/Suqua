#pragma once
#include <glad/glad.h>
#include <SDL.h>

#include <list>
#include <unordered_map>

#include "Sprite.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "ImgData.h"
#include "Particle.h"
#include "Pool.h"
#include "Particle.h"
#include "ComputeShader.h"
#include "ParticleSystem.h"

#define IMG_DATA_BUFFER_SIZE 1000

//add converter to point, test with player lc, use to test colliders.
struct Camera {
	Vec2f pos;
	Vec2f res;
	float camScale;

	void move(Vec2f move) {
		pos.x += move.x;
		pos.y += move.y;
	}

	void center(Vec2f center) {
		pos = { center.x - ((res.x / camScale) / 2), center.y - ((res.y / camScale) / 2) };
	}

	Vec2f center() {
		return { pos.x + (res.x / (2 * camScale)), pos.y + (res.y / (2 * camScale)) };
	}

	void zoom(float scale) {
		Vec2f center{ pos.x + (res.x / (2 * camScale)), pos.y + (res.y / (2 * camScale)) };
		camScale += scale;
		if (camScale < .1)
			camScale = .1;
		pos = { center.x - (res.x / (2 * camScale)), center.y - (res.y / (2 * camScale)) };
	}

	void setZoom(float scale) {
		Vec2f center{ pos.x + (res.x / (2 * camScale)), pos.y + (res.y / (2 * camScale)) };
		camScale = scale;
		if (camScale < .1)
			camScale = .1;
		pos = { center.x - (res.x / (2 * camScale)), center.y - (res.y / (2 * camScale)) };
	}

	bool inView(Vec2f pos_) {
		return (pos_.x > pos.x &&
			pos_.y > pos.y &&
			pos_.x < pos.x + res.x &&
			pos_.y < pos.y + res.y);
	}

	bool inView(Vec2f pos_, Vec2f res_) {
		return !(pos_.x > pos.x + res.x &&
			pos_.y > pos.y + res.y &&
			pos_.x + res_.x < pos.x &&
			pos_.y + res_.y < pos.y);
	}
};

struct RenderBuffer {
	unsigned int textureId;
	unsigned int shaderId;
	std::vector<ImgData> data;
};

enum DefShader {
	ImageShader = 0,
	DebugShader = 1,
	FullscreenShader = 2,
	ParticleShader = 3
};

class GLRenderer {
public:
	enum SelectType {
		include,
		exclude,
		all
	};

	//add support for buffer a ton of data from one texture.
	static void Init(SDL_Window* window, Vec2i windowRes, Vec2i viewRes);
	//static void Quit();

	static void Clear(GLbitfield bits);
	static void LoadShaders(const std::vector<std::pair<std::string, std::string>> & shaders, int * idsToFill);

	//Sets the shader to use, when data is buffered.
	static void SetShader(unsigned int id);
	//Sets the buffer to the specified style ID.
	static void SetBuffer(unsigned int styleId);
	//Place copy ImgData into the buffer
	static void Buffer(const ImgData& data);
	//set shader associated with a render buffer
	static void SetRenderBufferShader(unsigned int renderBufferId, unsigned int id);
	//Draw the selected RenderBuffers
	static void Draw(SelectType t_, std::vector<unsigned int>& ids);
	//Draw the selected RenderBuffers
	static void Draw(SelectType t_, int count = 0, unsigned int * ids = nullptr);
	//Clear the selected RenderBuffers
	static void ClearRenderBufs(SelectType t_, std::vector<unsigned int>& ids);
	//Clear the selected RenderBuffers
	static void ClearRenderBufs(SelectType t_, int number = 0, unsigned int* ids = nullptr);
	//Swap
	static void Swap();
	//Get Shader based on its stored id.
	static Shader& GetShaderRef(int id);
	//Get a default shader.
	static const Shader& GetDefaultShader(DefShader shader);
	//Set the current shader to one of the Default shaders.
	static void SetDefShader(DefShader shader);

	//Adds the specified texture/shader pair as its own buffer. Returns the location of the buffer in the RenderBuffer vector.
	static unsigned int GenRenderStyleBuf(unsigned int textureId, unsigned int shaderId);
	static unsigned int GenNewRenderStyleBuf(unsigned int textureId, unsigned int shaderId);
	//Resize specified buffer
	static void SetRenderBufSize(unsigned int id, int size);
	//render a single texture stretched over the whole screen (for post processing)
	static void DrawOverScreen(unsigned int texId);

	static PartitionID GenParticleType(unsigned int size, ComputeShader&& shader);
	static void SpawnParticles(PartitionID id, unsigned int count, Vec2f pos, const Particle& base);
	static void UpdateAndDrawParticles();

	static bool ReadErrors();

	static int addCamera(const Camera& cam);
	static void setCamera(int id);
	static Camera& getCamera(int id);
	static void bindCurrShader();
	static Vec2f screenToWorld(Vec2f point, int camId = currentCam);
	static Vec2f worldToSceen(Vec2f point, int camId = currentCam);
private:
	static unsigned int counter;
	static std::vector<RenderBuffer> renderBuffers;
	static SDL_Window* window;
	static SDL_GLContext context;
	static unsigned int VAO;
	static unsigned int ImgDataBuffer;
	static std::vector<Camera> cameras;
	static int currentCam;
	static Vec2i viewRes;
	static Vec2i windowRes;
	static RenderBuffer * currentRenderBuffer;
	static std::unordered_map<unsigned int, Shader> shaders;
	static Shader * currentShader;
	static int DefaultShaders[4];

	static const std::string Folder;
	static ParticleSystem particleSystem;
	
	//look in suqua, incomplete
};

/*
I think I'll abstract creating textures.
*/