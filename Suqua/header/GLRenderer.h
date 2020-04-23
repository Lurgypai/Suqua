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
#include "AABB.h"

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

struct Primitive {
	std::vector<Vec2f> points;
	float r;
	float g;
	float b;
};

struct RenderBuffer {
	unsigned int textureId;
	unsigned int shaderId;
	std::vector<ImgData> data;
};

enum DefShader {
	ImageShader = 0,
	TextShader = 1,
	FullscreenShader = 2,
	ParticleShader = 3,
	PrimitiveShader = 4
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
	//Draw everything thats been buffered since the last draw call
	static void DrawBuffered();
	//Draw a simple shape outline
	static void DrawPrimitve(std::vector<Vec2f> points, float r, float g, float b);
	//Draw an aabb outline
	static void DrawPrimitves(std::vector<AABB> rects, float r, float g, float b);
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
	static void DrawOverScreen(unsigned int texId, int width, int height);

	static PartitionID GenParticleType(const std::string & tag, int size, ComputeShader&& shader);
	static void SpawnParticles(PartitionID id, unsigned int count, const Particle& base, float angleModulation = 0, float velModulation = 0, int lifeModulation = 0, Vec2f posModulation = {0, 0});
	static void SpawnParticles(const std::string & tag, unsigned int count, const Particle& base, float angleModulation = 0, float velModulation = 0, int lifeModulation = 0, Vec2f posModulation = { 0, 0 });
	static void UpdateAndDrawParticles();
	static ComputeShader & getComputeShader(const std::string & tag);

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
	static unsigned int IMG_VAO;
	static unsigned int SCREEN_VAO;
	static unsigned int PRIMITIVE_VAO;
	static unsigned int VBO;
	static unsigned int ImgDataBuffer;
	static std::vector<Camera> cameras;
	static int currentCam;
	static Vec2i viewRes;
	static Vec2i windowRes;
	static RenderBuffer * currentRenderBuffer;
	static std::unordered_map<unsigned int, Shader> shaders;
	static Shader * currentShader;
	static int DefaultShaders[5];
	static std::vector<unsigned int> bufferedIds;
	static std::vector<Primitive> primitives;

	static const std::string Folder;
	static ParticleSystem particleSystem;
};

/*
I think I'll abstract creating textures.
*/