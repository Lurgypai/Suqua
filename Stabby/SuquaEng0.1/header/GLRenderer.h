#pragma once
#include <glad/glad.h>
#include <SDL.h>
#include "Sprite.h"
#include "Shader.h"
#include "ImgData.h"
#include <list>
#include <unordered_map>

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

class GLRenderer {
public:
	enum SelectType {
		include,
		exclude,
		all
	};

	//add support for buffer a ton of data from one texture.
	static void Init(SDL_Window* window);
	//static void Quit();

	static void Clear(GLbitfield bits);
	static void LoadShaders(const std::vector<std::pair<std::string, std::string>> & shaders, int * idsToFill);

	//Sets the shader to use, when data is buffered.
	static void SetShader(unsigned int id);
	//Sets the buffer to the specified style ID.
	static void SetBuffer(unsigned int styleId);
	//Place copy ImgData into the buffer
	static void Buffer(const ImgData& data);
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
	//Adds the specified texture/shader pair as its own buffer. Returns the location of the buffer in the RenderBuffer vector.
	static unsigned int GenRenderStyleBuf(unsigned int textureId, unsigned int shaderId);
	//Resize specified buffer
	static void SetRenderBufSize(unsigned int id, int size);


	static int addCamera(const Camera& cam);
	static void setCamera(int id);
	static Camera& getCamera(int id);
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
	static Vec2i windowRes;
	static RenderBuffer * currentRenderBuffer;
	static std::unordered_map<unsigned int, Shader> shaders;
	static Shader * currentShader;
	//look in suqua, incomplete
};

/*
I think I'll abstract creating textures.
*/