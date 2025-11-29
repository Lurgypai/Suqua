#include "ParticleEmitter.h"
#include "RandomUtil.h"
#include "Particle.h"
#include "GLRenderer.h"

ParticleEmitter::ParticleEmitter(Vec2f pos_, float radius_, std::string particleTag_, float emitDelay_) :
	pos{pos_},
	radius{radius_},
	particleTag{particleTag_},
	emitDelay{emitDelay_},
	on{true},
	elapsedTime{}
{}

void ParticleEmitter::update(float timeDelta) {
	if (!on) return;
	elapsedTime += timeDelta;
	if (elapsedTime < emitDelay) return;
	elapsedTime -= emitDelay;
	float angle = randFloat(0.f, 3.1415926535898f * 2.f);
	Vec2f offset{ radius, 0.f };
	offset.angle(angle);
	Vec2f spawnPos = pos + offset;
	Particle base{
		Color{ 1.f, 1.f, 1.f, 1.f },
		spawnPos,
		-90,
		.4f,
		20,
		0
	};
	GLRenderer::SpawnParticles(particleTag, 3, base, 0, 0, 0, Vec2f{1.f, 1.f});
}