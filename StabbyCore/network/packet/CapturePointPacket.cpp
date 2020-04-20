
#include "CapturePointPacket.h"

void CapturePointPacket::serialize() {
	netId = htonll(netId);

	remainingCaptureTime = htonf(state.remainingCaptureTime);
	totalCaptureTime = htonf(state.totalCaptureTime);
	state.currTeamId = htonll(state.currTeamId);
	state.targetTeamId = htonll(state.targetTeamId);

	pos.x = htonf(zone.pos.x);
	pos.y = htonf(zone.pos.y);
	res.x = htonf(zone.res.x);
	res.y = htonf(zone.res.y);
}

void CapturePointPacket::unserialize() {
	netId = ntohll(netId);

	state.remainingCaptureTime = ntohf(remainingCaptureTime);
	state.totalCaptureTime = ntohf(totalCaptureTime);
	state.currTeamId = ntohll(state.currTeamId);
	state.targetTeamId = ntohll(state.targetTeamId);

	zone.pos.x = ntohf(pos.x);
	zone.pos.y = ntohf(pos.y);
	zone.res.x = ntohf(res.x);
	zone.res.y = ntohf(res.y);
}
