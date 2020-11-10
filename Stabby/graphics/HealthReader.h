#pragma once
#include "StatBarComponent.h"

class HealthReader : public StatReader {
	friend class StatBarComponent;
public:
	virtual ~HealthReader() override;
	HealthReader(EntityId target_);
protected:
	virtual float getRatio() override;

	virtual StatReader* clone() override;
};