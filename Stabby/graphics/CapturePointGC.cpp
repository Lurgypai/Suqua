
#include "CapturePointGC.h"
#include "RenderComponent.h"
#include "RectDrawable.h"

#include "gamemode.h"

CapturePointGC::CapturePointGC(EntityId id_) :
	id{id_},
	currTime{0},
	currTick{0},
	colorSwapDelay{ 0.75 },
	currColor{}
{}

EntityId CapturePointGC::getId() const {
	return id;
}

void CapturePointGC::update(double timeDelta) {
	using CPState = CapturePointComponent::StateContainer;

	CapturePointComponent* capture = EntitySystem::GetComp<CapturePointComponent>(id);
	CPState state = capture->getState();

	switch (state.state)
	{
	case capturing:
	case contested:
		currTime += timeDelta;
		if (currTime > colorSwapDelay) {
			currTime = 0;
			++currTick;
		}
		if (currTick % 2) {
			currColor = teamColors[state.currTeamId];
		}
		else {
			currColor = teamColors[state.targetTeamId];
		}

		break;
	case idle:
		currColor = teamColors[state.currTeamId];
		break;
	default:
		break;
	}

	const auto& zone = capture->getZone();

	Particle p1{ currColor, {zone.center().x, zone.pos.y + zone.res.y}, -90, 0.1f, 100, 0 };
	GLRenderer::SpawnParticles("cap_zone", 1, p1, 0.0f, 0.2f, 0, {zone.res.x / 2, 0.0f });
}

const std::array<Color, 3> CapturePointGC::teamColors{ Color{0.5, 0.5, 0.5, 1.0}, Color{1.0, 0.0, 0.0, 1.0}, Color{0.0, 0.0, 1.0, 1.0} };