#pragma once
#include <glad/glad.h>
#include <SDL.h>

#include <list>
#include <unordered_map>
#include <memory>

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
#include "Color.h"

#define IMG_DATA_BUFFER_SIZE 1000

using CamId = uint32_t;

//add converter to point, test with player lc, use to test colliders.
struct Camera {
	Vec2f pos;
	Vec2i res;
	float camScale;

	void move(Vec2f move) {
		pos.x += move.x;
		pos.y += move.y;
	}

	void center(Vec2f center) {
		pos = { center.x - ((static_cast<float>(res.x) / camScale) / 2), center.y - ((static_cast<float>(res.y) / camScale) / 2)};
	}

	Vec2f center() const {
		return { pos.x + (res.x / (2 * camScale)), pos.y + (res.y / (2 * camScale)) };
	}

	void zoom(float scale) {
		Vec2f center{ pos.x + (res.x / (2 * camScale)), pos.y + (res.y / (2 * camScale)) };
		camScale += scale;
		if (camScale < .1f)
			camScale = .1f;
		pos = { center.x - (static_cast<float>(res.x) / (2 * camScale)), center.y - (static_cast<float>(res.y) / (2 * camScale))};
	}

	void setZoom(float scale) {
		Vec2f center{ pos.x + (res.x / (2 * camScale)), pos.y + (res.y / (2 * camScale)) };
		camScale = scale;
		if (camScale < .1f)
			camScale = .1f;
		pos = { center.x - (static_cast<float>(res.x) / (2 * camScale)), center.y - (static_cast<float>(res.y) / (2 * camScale)) };
	}

	bool inView(Vec2f pos_) const {
		return (pos_.x > pos.x &&
			pos_.y > pos.y &&
			pos_.x < pos.x + res.x &&
			pos_.y < pos.y + res.y);
	}

	bool inView(Vec2f pos_, Vec2f res_) const {
		return !(pos_.x > pos.x + res.x &&
			pos_.y > pos.y + res.y &&
			pos_.x + res_.x < pos.x &&
			pos_.y + res_.y < pos.y);
	}
};

struct Primitive {
	std::vector<Vec2f> points;
	float depth;
	Color color;
};

struct AtlasTextureData {
	const std::string tag;
	Vec2i offset;
	Vec2i res;
};

enum DefShader {
	ImageShader = 0,
	TextShader = 1,
	FullscreenShader = 2,
	ParticleShader = 3,
	PrimitiveShader = 4,
	CircleShader = 5,
};

class GLRenderer {
public:
	enum SelectType {
		include,
		exclude,
		all
	};

	//add support for buffer a ton of data from one texture.
	static void Init(SDL_Window* window, Vec2i windowRes);
	//static void Quit();

	static void Clear(GLbitfield bits);
	static void Clear();
	static void LoadShaders(const std::vector<std::pair<std::string, std::string>> & shaders, int * idsToFill);

	static void LoadTexture(const std::string& filePath, const std::string& tag);
	static AtlasTextureData GetTextureData(const std::string& tag);
	static void DrawTextureAtlas();

	static void BufferImgData(const ImgData& data);
	static void BufferImgData(ImgData&& data);

	static void DrawImage(ImgData data, const std::string& tag);

	//Sets the shader to use, when data is buffered.
	static void SetShader(unsigned int id);
	//Draw a simple shape outline
	static void DrawPrimitive(const Primitive& p);
	//Draw an aabb outline
	static void DrawPrimitives(const std::vector<Primitive>& p);

	static void DrawFilledPrimitive(const Primitive& p);
	static void DrawFilledPrimitives(const std::vector<Primitive>& p);

	//INCOMPLETE
	static void DrawCircle(const Vec2f& pos, float depth, float r, const Color& c);

	//Swap
	static void Swap();
	//Get Shader based on its stored id.
	static Shader& GetShaderRef(int id);
	//Get a default shader.
	static const Shader& GetDefaultShader(DefShader shader);
	//Set the current shader to one of the Default shaders.
	static void SetDefShader(DefShader shader);

	//render a single texture stretched over the whole screen (for post processing)
	static void DrawOverScreen(unsigned int texId);
	static void DrawOverScreen(unsigned int texId, int width, int height);

	static PartitionID GenParticleType(const std::string & tag, int size, ComputeShader&& shader);
	static void SpawnParticles(PartitionID id, unsigned int count, const Particle& base, float angleModulation = 0, float velModulation = 0, int lifeModulation = 0, Vec2f posModulation = {0, 0});
	static void SpawnParticles(const std::string & tag, unsigned int count, const Particle& base, float angleModulation = 0, float velModulation = 0, int lifeModulation = 0, Vec2f posModulation = { 0, 0 });
	static void UpdateAndDrawParticles();
	static ComputeShader & getComputeShader(const std::string & tag);
	static void BindTextureAtlas(unsigned int attachment_loc);

	static bool ReadErrors();

	static CamId addCamera(const Camera& cam);
	static void setCamera(CamId id);
	static Camera& getCamera(CamId id);
	static void bindCurrShader();
	static Vec2f screenToWorld(Vec2f point, int camId = currentCam);
	static Vec2f worldToSceen(Vec2f point, int camId = currentCam);
private:
	static unsigned int counter;
	static SDL_Window* window;
	static SDL_GLContext context;
	static unsigned int IMG_VAO;
	static unsigned int SCREEN_VAO;
	static unsigned int PRIMITIVE_VAO;
	static unsigned int CIRCLE_VAO;
	static unsigned int VBO;
	static unsigned int ImgDataBuffer;
	static std::vector<Camera> cameras;
	static int currentCam;
	static Vec2i windowRes;
	static std::unordered_map<unsigned int, Shader> shaders;
	static Shader * currentShader;
	static int DefaultShaders[6];
	static std::vector<unsigned int> bufferedIds;
	static std::vector<Primitive> primitives;

	static std::unique_ptr<Framebuffer> textureAtlas;
	static Vec2i textureAtlasPos;
	//how much to move down once we hit the end of the line;
	static unsigned int textureAtlasDropdown;
	static std::unordered_map<std::string, AtlasTextureData> textures;
	static std::vector<ImgData> imgData;

	static const std::string Folder;
	static ParticleSystem particleSystem;
};

/*
I think I'll abstract creating textures.
*/