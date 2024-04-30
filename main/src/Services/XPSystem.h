#ifndef BIT_FIRMWARE_XPSYSTEM_H
#define BIT_FIRMWARE_XPSYSTEM_H

#include "Periph/NVSFlash.h"

struct LevelProgress {
	uint8_t nextLvl; //from 2 to MaxLevel
	float progress; //0.0 - 1.0
};

class XPSystem {
public:
	XPSystem();
	uint32_t getXP() const;
	uint8_t getLevel() const;
	void increment(uint32_t xpGain);

	static constexpr LevelProgress MapXPToLevel(uint32_t xp);

private:
	NVSFlash& nvs;
	uint32_t xp = 0;

	void load();
	void store();

	static constexpr const char* BlobName = "XP";

	static constexpr uint8_t MaxLevel = 5;
	static constexpr uint32_t LevelupThresholds[MaxLevel - 1] = { 50, 200, 1000, 5000 };
};


#endif //BIT_FIRMWARE_XPSYSTEM_H
