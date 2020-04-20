#include <glad/glad.h>

#include "ParticleSystem.h"
#include "DebugIO.h"
#include "GLRenderer.h"
#include "RandomUtil.h"

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

PartitionID ParticleSystem::genPartition(const std::string & tag, int workGroupCount, ComputeShader && shader) {
	unsigned int size = workGroupCount * WORK_GROUP_SIZE;
	if (partitionOffset + size <= MAX_PARTICLES) {
		partitions.emplace_back(ParticlePartition{ size, 0, 0, partitionOffset, shader });
		partitionOffset += size;
		partitionIds.emplace(std::pair<std::string, PartitionID>{tag, partitions.size() - 1});
		return partitions.size() - 1;
	}
}

void ParticleSystem::spawnParticles(PartitionID id, unsigned int count, Particle base, float angleModulation, float velModulation, int lifeModulation, Vec2f posModulation) {
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
			auto & p = particles[i + offset];

			if (angleModulation != 0.0f)
				p.angle += randFloat(-angleModulation, angleModulation);
			if (velModulation != 0.0f)
				p.vel += randFloat(-velModulation, velModulation);
			if (lifeModulation != 0)
				p.life += randInt(-lifeModulation, lifeModulation);
			if (posModulation.x != 0.0f)
				p.pos.x += randFloat(-posModulation.x, posModulation.x);
			if (posModulation.y != 0.0f)
				p.pos.y += randFloat(-posModulation.y, posModulation.y);

		}
		for (int i = 0; i != end; ++i) {
			particles[i + offset] = base;
			auto & p = particles[i + offset];

			if (angleModulation != 0.0f)
				p.angle += randFloat(-angleModulation, angleModulation);
			if (velModulation != 0.0f)
				p.vel += randFloat(-velModulation, velModulation);
			if (lifeModulation != 0)
				p.life += randInt(-lifeModulation, lifeModulation);
			if (posModulation.x != 0.0f)
				p.pos.x += randFloat(-posModulation.x, posModulation.x);
			if (posModulation.y != 0.0f)
				p.pos.y += randFloat(-posModulation.y, posModulation.y);
		}
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticleDataBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * (start + offset), sizeof(Particle) * (size - start), &particles[start + offset]);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, sizeof(Particle) * end, &particles[offset]);
	}
	else if (end > start) {
		for (int i = start; i != end; ++i) {
			particles[i + offset] = base;
			auto & p = particles[i + offset];

			if (angleModulation != 0.0f)
				p.angle += randFloat(-angleModulation, angleModulation);
			if (velModulation != 0.0f)
				p.vel += randFloat(-velModulation, velModulation);
			if (lifeModulation != 0)
				p.life += randInt(-lifeModulation, lifeModulation);
			if (posModulation.x != 0.0f)
				p.pos.x += randFloat(-posModulation.x, posModulation.x);
			if (posModulation.y != 0.0f)
				p.pos.y += randFloat(-posModulation.y, posModulation.y);
		}
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ParticleDataBuffer);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * (start + offset), sizeof(Particle) * (end - start), &particles[start + offset]);
	}
}

void ParticleSystem::spawnParticles(const std::string & tag, unsigned int count, Particle base, float angleModulation, float velModulation, int lifeModulation, Vec2f posModulation) {
	if (partitionIds.find(tag) != partitionIds.end()) {
		spawnParticles(partitionIds[tag], count, base, angleModulation, velModulation, lifeModulation, posModulation);
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

	//DebugIO::setLine(3, "Particles: " + std::to_string(particleCount));
}

ComputeShader & ParticleSystem::getShader(PartitionID id) {
	return partitions[id].comp;
}

ComputeShader & ParticleSystem::getShader(const std::string & tag) {
	if (partitionIds.find(tag) != partitionIds.end()) {
		return partitions[partitionIds[tag]].comp;
	}
}
