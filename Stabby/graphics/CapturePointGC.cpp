
#include "CapturePointGC.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "RectDrawable.h"

#include "gamemode.h"

CapturePointGC::CapturePointGC(EntityId id_) :
	id{id_},
	currTime{0},
	currTick{0},
	colorSwapDelay{ 0.75 },
	currColor{}
{
	if (id != 0) {
		if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
			EntitySystem::MakeComps<RenderComponent>(1, &id);
			PositionComponent* pos = EntitySystem::GetComp<PositionComponent>(id);
			RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
			CapturePointComponent* capture = EntitySystem::GetComp<CapturePointComponent>(id);

			pos->pos = capture->getZone().pos;
			render->loadDrawable<RectDrawable>();
			render->getDrawable<RectDrawable>()->shape = capture->getZone();

			currColor = teamColors[capture->getState().currTeamId];

			render->getDrawable<RectDrawable>()->r = currColor.r;
			render->getDrawable<RectDrawable>()->g = currColor.g;
			render->getDrawable<RectDrawable>()->b = currColor.b;
		}
	}
}

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

	RenderComponent* render = EntitySystem::GetComp<RenderComponent>(id);
	render->getDrawable<RectDrawable>()->r = currColor.r;
	render->getDrawable<RectDrawable>()->g = currColor.g;
	render->getDrawable<RectDrawable>()->b = currColor.b;
}

const std::array<CapturePointGC::Color, 3> CapturePointGC::teamColors{ CapturePointGC::Color{0.5, 0.5, 0.5}, CapturePointGC::Color{1.0, 0.0, 0.0}, CapturePointGC::Color{0.0, 0.0, 1.0} };