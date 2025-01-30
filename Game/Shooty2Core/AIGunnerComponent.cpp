#include "AIGunnerComponent.h"
#include "ControllerComponent.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"
#include "RandomUtil.h"

#include "DebugIO.h"
#include <string>

using TeamId = TeamComponent::TeamId;
using AIState = AIGunnerComponent::AIState;

AIGunnerComponent::AIGunnerComponent(EntityId id_, float followRadius_, float avoidRadius_) :
    id{id_},
    followRadius{followRadius_},
    avoidRadius{avoidRadius_},

    idleTime{1.5f},
    walkTime{2.1f},
    followTime{3.9f},
    attackTime{2.f},

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

static inline void beginIdle(AIState& state, Controller& controller) {
    state = AIState::idle;
    controller.stick1 = {0.f, 0.f};
}

static inline void beginWalking(AIState& state, Controller& controller) {
    // set stick to random direction
    state = AIState::walking;

    constexpr float PI = 3.1415926535898;
    float dir = randFloat(0, PI * 2);
    controller.stick1 = Vec2f{.3f, 0.f};
    controller.stick1.angle(dir);
    controller.stick2 = Vec2f{1.f, 0.f};
    controller.stick2.angle(dir);
}

static inline void beginFollowing(AIState& state, float& angleMod) {
    state = AIState::following;

    angleMod = randFloat(-1.f, 1.f);
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
            if(targetId != 0) beginFollowing(state, angleMod);
            else beginWalking(state, controller);
            break; }
        case AIState::walking: {
            if (timeSinceLastAction < walkTime) break;
            timeSinceLastAction -= walkTime;


            targetId = findTarget(id, targetTeams, followRadius);
            if(targetId != 0) beginAttacking(state, controller);
            else beginIdle(state, controller);
            break; }
        case AIState::following: {
            if(timeSinceLastAction < followTime) {
                auto physicsComp = EntitySystem::GetComp<PhysicsComponent>(id);
                auto otherPhysicsComp = EntitySystem::GetComp<PhysicsComponent>(targetId);
                // entity has been removed
                if(otherPhysicsComp == nullptr) {
                    timeSinceLastAction = 0.0;
                    beginIdle(state, controller);
                    break;
                }

                Vec2f delta = otherPhysicsComp->center() - physicsComp->center();
                float moveMagn = 0.7f;
                if(delta.magn() < avoidRadius) moveMagn *= delta.magn() / avoidRadius - 1.f;
                else moveMagn *= (delta.magn() - avoidRadius) / (followRadius - avoidRadius);

                if(std::abs(moveMagn) < 0.1f) moveMagn = 0.f;

                float moveAngle = delta.angle() + angleMod;
                // float moveAngle = delta.angle();
                controller.stick1 = {1.f, 0.f};
                controller.stick1.angle(moveAngle);
                controller.stick1 *= moveMagn;
                controller.stick2 = delta.norm();

                break;
            }
            timeSinceLastAction -= followTime;

            targetId = findTarget(id, targetTeams, followRadius);
            if(targetId != 0) beginAttacking(state, controller);
            else beginIdle(state, controller);
            break; }
        case AIState::attacking:
            if(timeSinceLastAction < attackTime) {
                controller.stick1 = {};
                controller.off(ControllerBits::BUTTON_11);
                break;
            }
            timeSinceLastAction -= attackTime;
            beginFollowing(state, angleMod);
            break;
    }

    DirectionComponent* dir = EntitySystem::GetComp<DirectionComponent>(id);
    dir->setDir(controller.stick2.angle());

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
