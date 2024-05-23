#ifndef BIT_FIRMWARE_NVSUPGRADE_1_H
#define BIT_FIRMWARE_NVSUPGRADE_1_H

#include "Periph/NVSFlash.h"

class NVSUpgrade_1 : public NVSUpgrade {
public:
	inline virtual constexpr uint32_t getTargetVersion() const final override {
		return 2;
	}

	virtual void execute() const override;
};

#endif //BIT_FIRMWARE_NVSUPGRADE_1_H