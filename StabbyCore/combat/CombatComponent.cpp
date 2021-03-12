#include "RandomUtil.h"
#include "NetworkDataComponent.h"
#include "DirectionData.h"
#include "CombatComponent.h"
#include "CombatData.h"
#include <iostream>

using NDC = NetworkDataComponent;

//stop rebuilding
CombatComponent::CombatComponent(EntityId id_) :
	id{ id_ },
	stats{},
	ignoreDamage{false},
	staminaMax{ 700 },
	staminaRechargeMax{ 95 },
	freezeFrameMax{ 17 },
	attacks{AttackMap},
	attack{nullptr}
{
	if (id != 0) {
		if (!EntitySystem::Contains<NDC>() || EntitySystem::GetComp<NDC>(id) == nullptr) {
			EntitySystem::MakeComps<NDC>(1, &id);
		}

		NDC* data = EntitySystem::GetComp<NDC>(id);
		data->set<int32_t>(DIR, -1);
		data->set<bool>(INVULNERABLE, false);
		data->set<int32_t>(HEALTH, 0);
		data->set<uint32_t>(STUN_FRAME, 0);
		data->set<uint32_t>(TEAM_ID, 0);
		data->set<uint32_t>(STAMINA_RECHARGE_FRAME, 0);
		data->set<uint32_t>(FREEZE_FRAME, 0);
		data->set<uint32_t>(STAMINA, 0);
	}

	//set default attack
	attack = &(attacks.begin()->second);
}

CombatComponent::CombatComponent(const CombatComponent& other) :
	id{other.id},
	stats{other.stats},
	staminaMax{other.staminaMax},
	ignoreDamage{other.ignoreDamage},
	staminaRechargeMax{other.staminaRechargeMax},
	freezeFrameMax{other.freezeFrameMax},
	attacks{AttackMap},
	attack{nullptr},
	lastAttacker{other.lastAttacker},
	hurtboxes{other.hurtboxes},
	hitEntities{other.hitEntities}
{
	if (id != 0) {
		attack = &attacks[other.attack->getId()];
		NDC* data = EntitySystem::GetComp<NDC>(id);
		NDC* otherData = EntitySystem::GetComp<NDC>(other.getId());
		*data = *otherData;
	}
}

EntityId CombatComponent::getId() const {
	return id;
}

AABB CombatComponent::getBoundingBox() {
	Vec2f pos{}, res{};
	Vec2f min{ hurtboxes[0].box.pos }, max{ hurtboxes[0].box.pos + hurtboxes[0].box.res };
	for (auto & box : hurtboxes) {
		auto & hurtbox = box.box;
		min.x = hurtbox.pos.x < min.x ? hurtbox.pos.x : min.x;
		min.y = hurtbox.pos.y < min.x ? hurtbox.pos.y : min.y;
		max.x = hurtbox.pos.x + hurtbox.res.x > max.x ? hurtbox.pos.x + hurtbox.res.x : max.x;
		max.y = hurtbox.pos.y + hurtbox.res.y > max.y ? hurtbox.pos.y + hurtbox.res.y : max.y;
	}

	pos = min;
	res = max - min;

	return AABB(pos, res);
}

void CombatComponent::updateHurtboxes() {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);

	for (auto& hurtbox : hurtboxes) {
		hurtbox.box.pos = physics->getPos() + hurtbox.offset;
	}
}

void CombatComponent::updateStamina() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	auto& staminaRechargeFrame = data->get<uint32_t>(STAMINA_RECHARGE_FRAME);
	auto& stamina = data->get<uint32_t>(STAMINA);
	if (staminaRechargeFrame < staminaRechargeMax && attack->getActiveId() == 0) {
		++staminaRechargeFrame;
	}
	else if (staminaRechargeFrame == staminaRechargeMax) {
		if(stamina < staminaMax) {
			++stamina;
		}
	}
}

void CombatComponent::updateFreezeFrame() {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	if (data->get<uint32_t>(FREEZE_FRAME) < freezeFrameMax)
		++data->get<uint32_t>(FREEZE_FRAME);
}

void CombatComponent::onAttackLand()
{
}

void CombatComponent::onKill()
{
}

void CombatComponent::onDeath()
{
}

void CombatComponent::onHeal(unsigned int amount)
{
}

void CombatComponent::onDamage(unsigned int amount)
{
}

void CombatComponent::onStun(unsigned int amount) {
}

const Hitbox * CombatComponent::getActiveHitbox() const {
	return attack->getActive();
}

unsigned int CombatComponent::getStun() {
	auto activeAttack = attack->getActive();
	return activeAttack == nullptr ? 0 : activeAttack->stats.stun;
}

unsigned int CombatComponent::getStaminaCost() {
	auto activeAttack = attack->getActive();
	return activeAttack == nullptr ? 0 : activeAttack->stats.staminaCost;
}

bool CombatComponent::isStunned() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<uint32_t>(STUN_FRAME) != 0;
}

bool CombatComponent::isAlive() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<int32_t>(HEALTH) > 0;
}

bool CombatComponent::isFrozen() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	return data->get<uint32_t>(FREEZE_FRAME) != freezeFrameMax;
}

void CombatComponent::updateStun() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	auto& stunFrame = data->get<uint32_t>(STUN_FRAME);
	if (stunFrame > 0)
		--stunFrame;
	else
		stunFrame = 0;
}

void CombatComponent::damage(unsigned int i) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	if (!data->get<bool>(INVULNERABLE) && !ignoreDamage) {
		data->get<int32_t>(HEALTH) -= i;
		onDamage(i);
	}
}

void CombatComponent::heal(unsigned int i) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	auto& health = data->get<int32_t>(HEALTH);
	health += i;
	if (health > stats.maxHealth)
		health = stats.maxHealth;
	onHeal(i);
}

void CombatComponent::stun(unsigned int i) {
	NDC* data = EntitySystem::GetComp<NDC>(id);

	if(!data->get<bool>(INVULNERABLE))
		data->get<uint32_t>(STUN_FRAME) = i;
	onStun(i);
}

void CombatComponent::freeze() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	data->get<uint32_t>(STUN_FRAME) = 0;
}

bool CombatComponent::startAttacking() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	if (attack->canStartAttacking()) {
		if (data->get<uint32_t>(STAMINA) > 0) {
			attack->startAttacking();
			useStamina(attack->getHitbox(1)->stats.staminaCost);
			return true;
		}
	}
	return false;
}

bool CombatComponent::bufferNextAttack() {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	if (!attack->getNextIsBuffered()) {
		int currAttack = attack->getActiveId();
		auto nextHitbox = attack->getHitbox(currAttack + 1);
		if (nextHitbox) {
			if (data->get<uint32_t>(STAMINA) > 0) {
				attack->bufferNext();
				useStamina(nextHitbox->stats.staminaCost);
				return true;
			}
		}
	}
	return false;
}

bool CombatComponent::hasHitEntity(const EntityId& target) {
	return hitEntities.size() > target && hitEntities[target];
}

void CombatComponent::useStamina(uint32_t amount) {
	NDC* data = EntitySystem::GetComp<NDC>(id);
	auto& stamina = data->get<uint32_t>(STAMINA);
	stamina -= amount < stamina ? amount : stamina;
	data->get<uint32_t>(STAMINA_RECHARGE_FRAME) = 0;
}

void CombatComponent::setActiveHitbox(int i) {
	attack->setActive(i);
}

void CombatComponent::setFrame(int frame) {
	attack->setFrame(frame);
}

void CombatComponent::setAttackSpeed(double newSpeed) {
	attack->setSpeed(newSpeed);
}

void CombatComponent::setAttack(const std::string& attackId) {
	auto iter = attacks.find(attackId);
	if (iter != attacks.end())
		attack = &(iter->second);
	else
		throw std::exception{};
}

const Attack& CombatComponent::getAttack() const {
	if (attack)
		return *attack;
	else
		throw std::exception{};
}

void CombatComponent::addHitEntity(EntityId hit) {
	if (hitEntities.size() < hit + 1)
		hitEntities.resize(hit + 1);
	hitEntities.set(hit, true);
}

void CombatComponent::clearHitEntities() {
	hitEntities.zero();
}

int CombatComponent::rollDamage() {

	const Hitbox* active = attack->getActive();

	if (active != nullptr) {
		AttackStats attackStats = attack->getStats();

		float critChance = stats.critChance + attackStats.critChance;
		float critMultiplier = stats.critMultiplier + attackStats.critMultiplier;
		float damage = stats.baseDamage * attackStats.damage;

		float doCrit = randFloat(0.0f, 1.0f);
		int guaranteedCrits = static_cast<int>(stats.critChance);
		float remainingCritChance = stats.critChance - guaranteedCrits;

		//calculate the guaranteed crits, if the leftover chance happens do it, otherwise return the base damage.
		int totalDamage = (damage * critMultiplier) * guaranteedCrits + (doCrit < remainingCritChance ? damage * critMultiplier : damage);

		float vampirismChance = stats.vampirismChance + attackStats.vampirismChance;
		float vampirismMultiplier = stats.vampirismMultiplier + attackStats.vampirismMultiplier;

		float doVampire = randFloat(0.0f, 1.0f);
		if (doVampire < vampirismChance) {
			heal(totalDamage * vampirismMultiplier);
		}

		return totalDamage;
	}

	return 0;
}

EntityId CombatComponent::getLastAttacker() {
	return lastAttacker;
}

std::unordered_map<std::string, Attack> CombatComponent::AttackMap{};