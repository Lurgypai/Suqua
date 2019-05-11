#pragma once
#include <vector>

#include "Vec2.h"
#include "Particle.h"
#include "ComputeShader.h"

#define MAX_PARTICLES 2000000
#define WORK_GROUP_SIZE 1000

using PartitionID = unsigned int;

struct ParticlePartition {
	unsigned int size;
	unsigned int head;
	unsigned int tail;
	unsigned int offset;
	ComputeShader comp;
};

class ParticleSystem {
public:
	ParticleSystem();
	void genBuffer();
	//Create new particle partition. Specify the amount of allocated workgroups.
	PartitionID genPartition(unsigned int workGroupCount, ComputeShader&& shader);
	void spawnParticles(PartitionID id, unsigned int count, Vec2f pos, Particle base);
	void updateAndDraw(unsigned int camera);
private:
	unsigned int ParticleDataBuffer;
	unsigned int time;
	unsigned int partitionOffset;

	std::vector<Particle> particles;
	std::vector<ParticlePartition> partitions;
};