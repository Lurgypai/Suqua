#include "stdafx.h"
#include "GLRenderer.h"
#include "ImgData.h"

void GLRenderer::Init(SDL_Window * window_) {
	window = window_;
	SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	context = SDL_GL_CreateContext(window);

	gladLoadGLLoader(SDL_GL_GetProcAddress);

	SDL_GL_SetSwapInterval(0);
	
	SDL_GetWindowSize(window, &windowRes.x, &windowRes.y);
	glViewport(0, 0, windowRes.x, windowRes.y);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &ImgDataBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ImgDataBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ImgData) * IMG_DATA_BUFFER_SIZE, NULL, GL_STATIC_DRAW);

}

void GLRenderer::Clear(GLbitfield bits) {
	glClear(bits);
}

void GLRenderer::LoadShaders(const std::vector<std::pair<std::string, std::string>> & shaders_, int * idsToFill) {
	for (int i = 0; i != shaders_.size(); ++i) {
		Shader shader{ shaders_[i].first, shaders_[i].second };
		shaders.emplace(shader.id, shader);

		if (idsToFill != nullptr)
			idsToFill[i] = shader.id;
	}
}

void GLRenderer::SetShader(unsigned int id) {
	currentShader = &shaders[id];
}

void GLRenderer::SetBuffer(unsigned int styleId) {
	currentRenderBuffer = &renderBuffers[styleId];
}

//rework
void GLRenderer::Buffer(const ImgData& data) {
	currentRenderBuffer->shaderId = currentShader->id;
	currentRenderBuffer->data.push_back(data);
}

void GLRenderer::Draw(SelectType t_, std::vector<unsigned int> & ids) {
	Draw(t_, ids.size(), &ids[0]);
}

//select type spcifies how to draw
//include means draw only the specified texture buffers
//exclude means draw all texture buffers except the ones specified
//all means draw all texture buffers
void GLRenderer::Draw(SelectType t_, int count, unsigned int * ids) {
	Camera& cam = cameras[currentCam];

	//biind the vertex array object
	glBindVertexArray(VAO);

	std::vector<unsigned int> allIds;
	//handles what texture buffers to draw
	if (count > 0) {
		switch (t_) {
		case all:
			count = renderBuffers.size();
			for (int i = 0; i != renderBuffers.size(); ++i) {
				allIds.push_back(i);
			}
			ids = &allIds.front();
			break;
		case include:
			//already properly includes. count has how many were passed (and need to be included), 
			// the id points to the first of the include.
			//no change
			break;
		case exclude:
			//populate all ids, except for with the ones we don't want
			allIds.reserve(renderBuffers.size());
			for (int id = 0; id != renderBuffers.size(); ++id) {
				//loop through the passed texture ids
				for (int j = 0; j != count; ++j) {
					//store only the not equal ones
					if(ids[j] != id)
						allIds.push_back(id);
				}
			}
			//set count, and the starting ptr
			count = allIds.size();
			ids = &allIds.front();
			break;
		}
	}
	else {
		count = renderBuffers.size();
		for (int i = 0; i != renderBuffers.size(); ++i) {
			allIds.push_back(i);
		}
		ids = &allIds.front();
	}

	//loop through the specified texture buffers and draw
	for (int j = 0; j != count; ++j) {
		RenderBuffer & renderBuf = renderBuffers[ids[j]];
		std::vector<ImgData> & textureBuf = renderBuf.data;

		shaders[renderBuf.shaderId].use();
		shaders[renderBuf.shaderId].uniform2f("camPos", cam.pos.x, cam.pos.y);
		shaders[renderBuf.shaderId].uniform2f("camRes", cam.res.x, cam.res.y);
		shaders[renderBuf.shaderId].uniform2f("zoom", cam.camScale, cam.camScale);

		//grab the size
		int size = static_cast<int>(textureBuf.size());
		//bind the range for the first IMG_DATA_BUFFER_SIZE
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ImgDataBuffer, 0, sizeof(ImgData) * IMG_DATA_BUFFER_SIZE);
		//loop through batches of IMG_DATA_BUFFER_SIZE ImgDatas
		for (int i = 0; i != ceil(static_cast<float>(size) / IMG_DATA_BUFFER_SIZE); i++) {

			int objsToBuffer{ size - (i * IMG_DATA_BUFFER_SIZE) };
			if (objsToBuffer > IMG_DATA_BUFFER_SIZE)
				objsToBuffer = IMG_DATA_BUFFER_SIZE;

			//buffer in as many as there are
			glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(ImgData) * objsToBuffer, &(textureBuf.front()) + (IMG_DATA_BUFFER_SIZE * i));

			//bind the texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, renderBuf.textureId);

			//draw. 6 vertices (should) make a rectangle
			glDrawArrays(GL_TRIANGLES, 0, 6 * objsToBuffer);
		}
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		//glBindVertexArray(0);
	}
}

void GLRenderer::Swap() {
	SDL_GL_SwapWindow(window);
}

Shader & GLRenderer::GetShaderRef(int id) {
	return shaders[id];
}

unsigned int GLRenderer::GenRenderStyleBuf(unsigned int id, unsigned int shaderId) {
	renderBuffers.push_back(RenderBuffer{ id, shaderId, {} });
	return renderBuffers.size() - 1;
}

void GLRenderer::SetRenderBufSize(unsigned int id, int size) {
	renderBuffers[id].data.reserve(size);
}

void GLRenderer::ClearRenderBufs(SelectType t_, std::vector<unsigned int>& ids) {
	ClearRenderBufs(t_, ids.size(), &ids.front());
}

void GLRenderer::ClearRenderBufs(SelectType t_, int count, unsigned int * ids) {
	switch (t_) {
	case all:
		for (auto& renderBuf : renderBuffers) {
			int size = renderBuf.data.size();;
			renderBuf.data.clear();
			renderBuf.data.reserve(size);
		}
		break;
	case include:
		for (int j = 0; j != count; ++j) {
			int size = renderBuffers[ids[j]].data.size();
			renderBuffers[ids[j]].data.clear();
			renderBuffers[ids[j]].data.reserve(size);
		}
		break;
	case exclude:
		for (int j = 0; j != count; ++j) {
			for (int id = 0; id != renderBuffers.size(); ++id) {
				if (id != ids[j]) {
					int size = renderBuffers[id].data.size();
					renderBuffers[id].data.clear();
					renderBuffers[id].data.reserve(size);
				}
			}
		}
		break;
	}
}

int GLRenderer::addCamera(const Camera & cam) {
	cameras.push_back(cam);
	return cameras.size() - 1;
}

void GLRenderer::setCamera(int id) {
	currentCam = id;
}

Camera& GLRenderer::getCamera(int id) {
	return cameras[id];
}

Vec2f GLRenderer::screenToWorld(Vec2f point, int camId) {

	Camera& cam = cameras[camId];
	
	point.x /= windowRes.x;
	point.y /= windowRes.y;
	point.x *= cam.res.x / cam.camScale;
	point.y *= cam.res.y / cam.camScale;
	point.x += cam.pos.x;
	point.y += cam.pos.y;
	return point;
}

Vec2f GLRenderer::worldToSceen(Vec2f point, int camId) {
	Camera& cam = cameras[camId];

	point.x -= cam.pos.x;
	point.y -= cam.pos.y;
	point.x /= cam.res.x / cam.camScale;
	point.y /= cam.res.y / cam.camScale;
	point.x *= windowRes.x;
	point.y *= windowRes.y;
	return point;
}

std::vector<RenderBuffer> GLRenderer::renderBuffers{};
SDL_GLContext GLRenderer::context{};
SDL_Window* GLRenderer::window{nullptr};
unsigned int GLRenderer::VAO{};
unsigned int GLRenderer::ImgDataBuffer{};
std::vector<Camera> GLRenderer::cameras{};
int GLRenderer::currentCam{};
Vec2i GLRenderer::windowRes{};
RenderBuffer * GLRenderer::currentRenderBuffer{};
std::unordered_map<unsigned int, Shader> GLRenderer::shaders{};
Shader * GLRenderer::currentShader{nullptr};