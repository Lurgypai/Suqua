#include "stdafx.h"

#include <glad/glad.h>

#include "ParticleSystem.h"
#include "DebugIO.h"
#include "GLRenderer.h"

ParticleSystem::ParticleSystem() :
	ParticleDataBuffer{ 0 },
	particles{}
{
	particles.resize(MAX_PARTICLES);
}

void ParticleSystem::genBuffer() {
	glGenBuffers(1, &ParticleDataBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticleDataBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * MAX_PARTICLES, NULL, GL_STATIC_DRAW);
}

PartitionID ParticleSystem::genPartition(unsigned int workGroupCount, ComputeShader && shader) {
	unsigned int size = workGroupCount * WORK_GROUP_SIZE;
	if (partitionOffset + size <= MAX_PARTICLES) {
		partitions.emplace_back(ParticlePartition{ size, 0, 0, partitionOffset, shader });
		partitionOffset += size;
		return partitions.size() - 1;
	}
}

void ParticleSystem::spawnParticles(PartitionID id, unsigned int count, Vec2f pos, Particle base) {
	ParticlePartition & partition = partitions[id];
	auto & head = partition.head;
	auto & tail = partition.tail;
	auto size = partition.size;
	auto offset = partition.offset;

	base.death = time + base.life;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ParticleDataBuffer);

	//if overflow, bail
	if ((tail + count) - head > size)
		return;

	int start = tail % size;
	tail += count;
	int end = tail % size;

	if (end < start) {
		for (int i = start; i != size; ++i) {
			particles[i + offset] = base;
		}
		for (int i = 0; i != end; ++i) {
			particles[i + offset] = base;
		}
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticleDataBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * (start + offset), sizeof(Particle) * (size - start), &particles[start + offset]);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Particle) * end, &particles[offset]);
	}
	else if (end > start) {
		for (int i = start; i != end; ++i) {
			particles[i + offset] = base;
		}
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticleDataBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * (start + offset), sizeof(Particle) * (end - start), &particles[start + offset]);
	}
}

void ParticleSystem::updateAndDraw(unsigned int camera) {
	++time;
	unsigned int particleCount{0};
	for (auto& partition : partitions) {
		auto & head = partition.head;
		auto & tail = partition.tail;
		auto size = partition.size;
		auto offset = partition.offset;

		int i = head % size;
		while (particles[i + offset].death < time && head < tail) {
			head++;
			i = head % size;
		}

		particleCount += tail - head;

		partition.comp.use();
		glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, ParticleDataBuffer, offset * sizeof(Particle), size * sizeof(Particle));

		glDispatchCompute(size / WORK_GROUP_SIZE, 1, 1);

		Camera& cam = GLRenderer::getCamera(camera);
		const Shader& particleShader = GLRenderer::GetDefaultShader(ParticleShader);

		particleShader.use();
		particleShader.uniform2f("camPos", cam.pos.x, cam.pos.y);
		particleShader.uniform2f("camRes", cam.res.x, cam.res.y);
		particleShader.uniform2f("zoom", cam.camScale, cam.camScale);

		int start = head % size;
		particleShader.uniform1i("start", start);
		particleShader.uniform1i("size", size);

		glDrawArrays(GL_POINTS, 0, tail - head);
	}

	DebugIO::setLine(3, "Particles: " + std::to_string(particleCount));
}
