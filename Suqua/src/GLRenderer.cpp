#include "GLRenderer.h"
#include "ImgData.h"
#include "Particle.h"
#include "RandomUtil.h"
#include "DebugIO.h"
#include <iostream>

void GLRenderer::Init(SDL_Window * window_, Vec2i windowRes_, Vec2i viewRes_) {
	window = window_;
	windowRes = windowRes_;
	viewRes = viewRes_;
	SDL_GL_LoadLibrary(NULL);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	context = SDL_GL_CreateContext(window);


	gladLoadGLLoader(SDL_GL_GetProcAddress);


	SDL_GL_SetSwapInterval(1);
	
	glViewport(0, 0, windowRes.x, windowRes.y);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &PRIMITIVE_VAO);
	glGenVertexArrays(1, &IMG_VAO);
	glGenVertexArrays(1, &SCREEN_VAO);
	glGenBuffers(1, &ImgDataBuffer);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ImgDataBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(ImgData) * IMG_DATA_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glBindVertexArray(PRIMITIVE_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	particleSystem.genBuffer();

	GLRenderer::LoadShaders({
		{Folder + "shaders/image.vert", Folder + "shaders/image.frag"},
		{Folder + "shaders/image.vert", Folder + "shaders/text.frag"},
		{Folder + "shaders/basic.vert", Folder + "shaders/basic.frag"},
		{Folder + "shaders/particle.vert", Folder + "shaders/particle.frag"},
		{Folder + "shaders/primitive.vert", Folder + "shaders/primitive.frag"}
		}, &DefaultShaders[0]);
	GLRenderer::GetShaderRef(DefaultShaders[ImageShader]).use();
	GLRenderer::GetShaderRef(DefaultShaders[ImageShader]).uniform2f("windowRes", windowRes.x, windowRes.y);
	GLRenderer::GetShaderRef(DefaultShaders[TextShader]).use();
	GLRenderer::GetShaderRef(DefaultShaders[TextShader]).uniform2f("windowRes", windowRes.x, windowRes.y);
	GLRenderer::GetShaderRef(DefaultShaders[ParticleShader]).use();
	GLRenderer::GetShaderRef(DefaultShaders[ParticleShader]).uniform2f("windowRes", windowRes.x, windowRes.y);
	GLRenderer::GetShaderRef(DefaultShaders[PrimitiveShader]).use();
	GLRenderer::GetShaderRef(DefaultShaders[PrimitiveShader]).uniform2f("windowRes", windowRes.x, windowRes.y);
}

void GLRenderer::Clear(GLbitfield bits) {
	glClear(bits);
}

void GLRenderer::LoadShaders(const std::vector<std::pair<std::string, std::string>> & shaders_, int * idsToFill) {
	shaders.reserve(shaders.size() + shaders_.size());
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

void GLRenderer::SetRenderBufferShader(unsigned int renderBufferId, unsigned int id) {
	renderBuffers[renderBufferId].shaderId = id;
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

	if (renderBuffers.size() > 0) {
		Camera& cam = cameras[currentCam];

		//biind the vertex array object
		glViewport(0, 0, cam.res.x, cam.res.y);
		glBindVertexArray(IMG_VAO);
		glDisableVertexAttribArray(0);

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
						if (ids[j] != id)
							allIds.push_back(id);
					}
				}
				//set count, and the starting ptr
				if (allIds.size() > 0) {
					count = allIds.size();
					ids = &allIds.front();
				}
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
			if (!textureBuf.empty()) {

				shaders[renderBuf.shaderId].use();
				shaders[renderBuf.shaderId].uniform2f("camPos", cam.pos.x, cam.pos.y);
				shaders[renderBuf.shaderId].uniform2f("camRes", cam.res.x, cam.res.y);
				shaders[renderBuf.shaderId].uniform2f("zoom", cam.camScale, cam.camScale);

				//grab the size
				int size = static_cast<int>(textureBuf.size());
				//bind the range for the first IMG_DATA_BUFFER_SIZE
				glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ImgDataBuffer);
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
			}
		}
		glBindVertexArray(0);
	}
}

void GLRenderer::DrawPrimitve(std::vector<Vec2f> points, float r, float g, float b) {

	GLRenderer::SetDefShader(PrimitiveShader);

	Camera& cam = cameras[currentCam];

	glViewport(0, 0, cam.res.x, cam.res.y);
	glBindVertexArray(PRIMITIVE_VAO);

	currentShader->use();
	currentShader->uniform2f("camPos", cam.pos.x, cam.pos.y);
	currentShader->uniform2f("camRes", cam.res.x, cam.res.y);
	currentShader->uniform2f("zoom", cam.camScale, cam.camScale);
	currentShader->uniform3f("color", r, g, b);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Vec2f), points.data(), GL_DYNAMIC_DRAW);

	/*
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vec2f), (void*)0);
	glEnableVertexAttribArray(0);
	*/

	glDrawArrays(GL_LINE_LOOP, 0, points.size());

	glBindVertexArray(0);
}

void GLRenderer::DrawPrimitves(std::vector<AABB> rects, float r, float g, float b) {
	for (auto& shape : rects) {
		GLRenderer::DrawPrimitve({ shape.pos, Vec2f{shape.pos.x + shape.res.x, shape.pos.y}, shape.pos + shape.res, Vec2f{shape.pos.x, shape.pos.y + shape.res.y} }, r, g, b);
	}
}

void GLRenderer::Swap() {
	SDL_GL_SwapWindow(window);
}

Shader & GLRenderer::GetShaderRef(int id) {
	return shaders[id];
}

const Shader & GLRenderer::GetDefaultShader(DefShader shader) {
	return shaders[DefaultShaders[shader]];
}

void GLRenderer::SetDefShader(DefShader shader) {
	currentShader = &shaders[DefaultShaders[shader]];
}

unsigned int GLRenderer::GenRenderStyleBuf(unsigned int id, unsigned int shaderId) {
	for (int i = 0; i != renderBuffers.size(); ++i) {
		auto& renderBuf = renderBuffers[i];
		if (renderBuf.textureId == id)
			return i;
	}
	renderBuffers.push_back(RenderBuffer{ id, shaderId, {} });
	return renderBuffers.size() - 1;
}


unsigned int GLRenderer::GenNewRenderStyleBuf(unsigned int textureId, unsigned int shaderId) {
	renderBuffers.push_back(RenderBuffer{ textureId, shaderId, {} });
	return renderBuffers.size() - 1;
}

void GLRenderer::SetRenderBufSize(unsigned int id, int size) {
	renderBuffers[id].data.reserve(size);
}

void GLRenderer::DrawOverScreen(unsigned int texId) {
	glBindVertexArray(SCREEN_VAO);
	glViewport(0, 0, windowRes.x, windowRes.y);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void GLRenderer::DrawOverScreen(unsigned int texId, int width, int height) {
	glBindVertexArray(SCREEN_VAO);
	glViewport(0, 0, width, height);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

PartitionID GLRenderer::GenParticleType(const std::string & tag, int size, ComputeShader && shader) {
	return particleSystem.genPartition(tag, size, std::forward<ComputeShader>(shader));
}

void GLRenderer::SpawnParticles(PartitionID id, unsigned int count, const Particle & base, float angleModulation, float velModulation, int lifeModulation, Vec2f posModulation) {
	particleSystem.spawnParticles(id, count, base, angleModulation, velModulation, lifeModulation, posModulation);
}

void GLRenderer::SpawnParticles(const std::string & tag, unsigned int count, const Particle & base, float angleModulation, float velModulation, int lifeModulation, Vec2f posModulation) {
	particleSystem.spawnParticles(tag, count, base, angleModulation, velModulation, lifeModulation, posModulation);
}

void GLRenderer::UpdateAndDrawParticles() {
	particleSystem.updateAndDraw(currentCam);
}

ComputeShader & GLRenderer::getComputeShader(const std::string & tag) {
	return particleSystem.getShader(tag);
}

bool GLRenderer::ReadErrors() {
	GLenum error = glGetError();
	bool ret = false;
	while (error) {
		ret = true;
		std::cout << "Error detected: ";
		switch (error) {
		case GL_INVALID_ENUM:
			std::cout << "Invalid Enum";
			break;
		case GL_INVALID_VALUE:
			std::cout << "Invalid Value";
			break;
		case GL_INVALID_OPERATION:
			std::cout << "Invalid Operation";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "Invalid Framebuffer Operation";
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "Out of Memory";
			break;
		case GL_STACK_UNDERFLOW:
			std::cout << "Stack Underflow";
			break;
		case GL_STACK_OVERFLOW:
			std::cout << "Stack Overflow";
			break;
		}
		std::cout << '\n';
		error = glGetError();
	}
	return ret;
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

void GLRenderer::bindCurrShader() {
	currentShader->use();
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
unsigned int GLRenderer::SCREEN_VAO{};
unsigned int GLRenderer::IMG_VAO{};
unsigned int GLRenderer::PRIMITIVE_VAO{};
unsigned int GLRenderer::VBO{};
unsigned int GLRenderer::ImgDataBuffer{};
std::vector<Camera> GLRenderer::cameras{};
int GLRenderer::currentCam{};
Vec2i GLRenderer::windowRes{};
Vec2i GLRenderer::viewRes{};
RenderBuffer * GLRenderer::currentRenderBuffer{};
std::unordered_map<unsigned int, Shader> GLRenderer::shaders{};
Shader * GLRenderer::currentShader{nullptr};
int GLRenderer::DefaultShaders[5]{};
ParticleSystem GLRenderer::particleSystem{};

const std::string GLRenderer::Folder{"suqua/"};