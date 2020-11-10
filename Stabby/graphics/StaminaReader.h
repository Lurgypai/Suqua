#include "StatBarComponent.h"

class StaminaReader : public StatReader {
	friend class StatBarComponent;
public:
	virtual ~StaminaReader() override;
	StaminaReader(EntityId target_);
protected:
	virtual float getRatio() override;

	virtual StatReader* clone() override;
};