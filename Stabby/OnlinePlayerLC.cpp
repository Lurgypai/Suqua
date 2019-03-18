#include "pch.h"
#include "OnlinePlayerLC.h"
#include <iostream>
#include "SDL.h"
#include "ServerClientData.h"

OnlinePlayerLC::OnlinePlayerLC(EntityId id_) :
	id{ id_ },
	netId{ 0 },
	vel{0, 0},
	pos{ 0.0f, 0.0f },
	previousPos{},
	whens{},
	attackId{0},
	facing{1}
{}

void OnlinePlayerLC::setNetId(NetworkId id_) {
	netId = id_;
}

NetworkId OnlinePlayerLC::getNetId() {
	return netId;
}

EntityId OnlinePlayerLC::getId() const {
	return id;
}

Vec2f OnlinePlayerLC::getPos() const {
	return pos;
}

Vec2f OnlinePlayerLC::getVel() const {
	return vel;
}

void OnlinePlayerLC::interp(PlayerState st, Time_t when) {
	if (vel.x < 0)
		facing = -1;
	else if(vel.x > 0)
		facing = 1;

	vel = st.vel;
	state = st.state;
	attackId = st.activeAttack;

	previousPos[0] = previousPos[1];
	previousPos[1] = previousPos[2];
	previousPos[2] = st.pos;

	whens[0] = whens[1];
	whens[1] = whens[2];
	whens[2] = when;

	pos = previousPos[1];

}

void OnlinePlayerLC::update(Time_t gameTime) {
	//switch to server ticks
	//this is how long we want to tak to get from where we are to where we are going
	int front = 2;
	if (gameTime > whens[front] && (gameTime - whens[front]) * GAME_TIME_STEP < 0.1)
		front = 1;


	double delta = static_cast<double>(whens[front] - whens[front - 1]) * SERVER_TIME_STEP;
	//this is how long it is between each update.

	Vec2f moveDistance = (previousPos[front] - previousPos[front - 1]) * static_cast<float>(CLIENT_TIME_STEP / delta);
	pos += moveDistance;
}

int OnlinePlayerLC::getActiveId() {
	return attackId;
}

int OnlinePlayerLC::getFacing() {
	return facing;
}

State OnlinePlayerLC::getState() {
	return state;
}

std::string OnlinePlayerLC::getHeadPath() {
	return "images/bad_head.png";
}
