#ifndef BIT_FIRMWARE_NVSUPGRADES_H
#define BIT_FIRMWARE_NVSUPGRADES_H

#include <vector>
#include "NVSUpgrade_1.h"

inline static const std::vector<NVSUpgrade*> NVSUpgrades = { new NVSUpgrade_1() };

#endif //BIT_FIRMWARE_NVSUPGRADES_H