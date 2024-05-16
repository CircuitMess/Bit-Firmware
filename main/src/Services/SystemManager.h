#ifndef BIT_FIRMWARE_SYSTEMMANAGER_H
#define BIT_FIRMWARE_SYSTEMMANAGER_H

#include <cstdint>
#include "Periph/NVSFlash.h"

class SystemManager {
public:
	inline static constexpr const uint32_t CurrentVersion = 2;

public:
	explicit SystemManager(const std::vector<NVSUpgrade>& upgrades);
};

#endif //BIT_FIRMWARE_SYSTEMMANAGER_H