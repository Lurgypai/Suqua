#include "CapturePointPacket.h"
#include "ByteOrder.h"

void CapturePointPacket::serialize() {
	netId = s_htonll(netId);

	remainingCaptureTime = s_htonf(state.remainingCaptureTime);
	totalCaptureTime = s_htonf(state.totalCaptureTime);
	state.currTeamId = s_htonll(state.currTeamId);
	state.targetTeamId = s_htonll(state.targetTeamId);

	pos.x = s_htonf(zone.pos.x);
	pos.y = s_htonf(zone.pos.y);
	res.x = s_htonf(zone.res.x);
	res.y = s_htonf(zone.res.y);
}

void CapturePointPacket::unserialize() {
	netId = s_ntohll(netId);

	state.remainingCaptureTime = s_ntohf(remainingCaptureTime);
	state.totalCaptureTime = s_ntohf(totalCaptureTime);
	state.currTeamId = s_ntohll(state.currTeamId);
	state.targetTeamId = s_ntohll(state.targetTeamId);

	zone.pos.x = s_ntohf(pos.x);
	zone.pos.y = s_ntohf(pos.y);
	zone.res.x = s_ntohf(res.x);
	zone.res.y = s_ntohf(res.y);
}
