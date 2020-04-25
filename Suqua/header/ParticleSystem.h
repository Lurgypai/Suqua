#pragma once
#include <vector>
#include <unordered_map>

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
	PartitionID genPartition(const std::string & tag, int workGroupCount, ComputeShader&& shader);
	void spawnParticles(PartitionID id, unsigned int count, Particle base, float angleModulation = 0, float velModulation = 0, int lifeModulation = 0, Vec2f posModulation = {0, 0});
	void spawnParticles(const std::string & tag, unsigned int count, Particle base, float angleModulation = 0, float velModulation = 0, int lifeModulation = 0, Vec2f posModulation = {0, 0});
	void updateAndDraw(unsigned int camera);
	ComputeShader & getShader(PartitionID id);
	ComputeShader & getShader(const std::string & tag);
private:
	unsigned int PARTICLE_VAO;

	unsigned int ParticleDataBuffer;
	unsigned int time;
	unsigned int partitionOffset;

	std::vector<Particle> particles;
	std::vector<ParticlePartition> partitions;
	std::unordered_map<std::string, PartitionID> partitionIds;
};