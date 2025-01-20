#include "AIGunnerComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "RandomUtil.h"

#include "DebugIO.h"
#include <string>

using TeamId = TeamComponent::TeamId;
using AIState = AIGunnerComponent::AIState;

AIGunnerComponent::AIGunnerComponent(EntityId id_, float followRadius_, float avoidRadius_) :
    id{id_},
    followRadius{followRadius_},
    avoidRadius{avoidRadius_},

    idleTime{0.7f},
    walkTime{0.5f},
    followTime{2.1f},
    attackTime{0.9f},

    timeSinceLastAction{0.f},
    targetId{0},
    targetTeams{TeamId::player},
    state{AIState::idle}
{}

static inline EntityId findTarget(EntityId id, const std::set<TeamId> targetTeams, float followRadius) {
    for(auto& teamComp : EntitySystem::GetPool<TeamComponent>()) {
        if(teamComp.getId() == id) continue;
        if(!targetTeams.contains(teamComp.teamId)) continue;
        auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(teamComp.getId());
        if(physicsComp == nullptr) continue;
        auto targetPos = physicsComp->position();

        auto ourPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
        if(ourPhysicsComp->position().distance(targetPos) < followRadius) return teamComp.getId();
    }
    return 0;
}

static inline void beginIdle(AIState& state) {
    state = AIState::idle;
}

static inline void beginWalking(AIState& state, Controller& controller) {
    // set stick to random direction
    state = AIState::walking;

    constexpr float PI = 3.1415926535898;
    float dir = randFloat(0, PI * 2);
    controller.stick1.angle(dir);
}

static inline void beginFollowing(AIState& state) {
    state = AIState::following;
}

static inline void beginAttacking(AIState& state, Controller& controller) {
    state = AIState::attacking;

    // do attack
    controller.on(ControllerBits::BUTTON_11);
}

void AIGunnerComponent::update(double delta) {
    // idle
    //  sit and do nothing
    // walking
    //  walk in decided direction
    // following
    //  follow player, count down to attack
    // attacking
    //  do attack input, wait.
    auto contComp = EntitySystem::GetComp<ControllerComponent>(id);
    auto& controller = contComp->getController();
    timeSinceLastAction += delta;

    switch(state) {
        case AIState::idle: {
            if (timeSinceLastAction < idleTime) break;
            timeSinceLastAction -= idleTime;


            targetId = findTarget(id, targetTeams, followRadius);
            if(targetId != 0) beginFollowing(state);
            else beginWalking(state, controller);
            break; }
        case AIState::walking: {
            if (timeSinceLastAction < walkTime) break;
            timeSinceLastAction -= walkTime;


            targetId = findTarget(id, targetTeams, followRadius);
            if(targetId != 0) beginAttacking(state, controller);
            else beginIdle(state);
            break; }
        case AIState::following: {
            if(timeSinceLastAction < followTime) {
                auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
                auto otherPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(targetId);
                // entity has been removed
                if(otherPhysicsComp == nullptr) {
                    timeSinceLastAction = 0.0;
                    beginIdle(state);
                    break;
                }

                auto dir = (otherPhysicsComp->position() - physicsComp->position()).norm() * 0.3f;
                controller.stick1 = dir;
                controller.stick2 = dir;
                break;
            }
            timeSinceLastAction -= followTime;

            targetId = findTarget(id, targetTeams, followRadius);
            if(targetId != 0) beginAttacking(state, controller);
            else beginIdle(state);
            break; }
        case AIState::attacking:
            if(timeSinceLastAction < attackTime) {
                controller.stick1 = {};
                controller.set(ControllerBits::BUTTON_11, false);
                break;
            }
            timeSinceLastAction -= attackTime;
            beginFollowing(state);
            break;
    }

    std::string strState;
    switch(state) {
        case AIState::idle: strState = "idle"; break;
        case AIState::walking: strState = "walking"; break;
        case AIState::following: strState = "following"; break;
        case AIState::attacking: strState = "attacking"; break;
    }
    DebugIO::setLine(5, "State: " + strState);
    DebugIO::setLine(6, "Stick1: " + std::to_string(controller.stick1.angle()));
    DebugIO::setLine(7, "Stick2: " + std::to_string(controller.stick2.angle()));
    DebugIO::setLine(8, "Attack: " + std::to_string(controller[ControllerBits::BUTTON_11]));
    DebugIO::setLine(9, "Attack Toggled? " + std::to_string(controller.toggled(ControllerBits::BUTTON_11)));
}
