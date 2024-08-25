#ifndef BIT_FIRMWARE_XPSYSTEM_H
#define BIT_FIRMWARE_XPSYSTEM_H

#include "Periph/NVSFlash.h"

struct LevelProgress {
	uint8_t nextLvl; //from 2 to MaxLevel
	float progress; //0.0 - 1.0
	uint8_t currLvl;
};

class XPSystem {
public:
	XPSystem();
	uint32_t getXP() const;
	uint8_t getLevel() const;
	void increment(uint32_t xpGain);

	inline static constexpr LevelProgress MapXPToLevel(uint32_t xp) {
		const uint8_t levelupsNum = sizeof(LevelupThresholds) / sizeof(uint32_t);
		uint8_t curLvl = 1;

		for(uint8_t i = 0; i < levelupsNum; i++){
			if(xp < LevelupThresholds[i]){
				return { (uint8_t) (i + 2), (float) xp / LevelupThresholds[i], curLvl };
			}

			xp -= LevelupThresholds[i];
			curLvl++;
		}
		return { MaxLevel, 1.0f, 5 };
	}

	static constexpr uint8_t MaxLevel = 5;

private:
	NVSFlash& nvs;
	uint32_t xp = 0;

	void load();
	void store();

	static constexpr const char* BlobName = "XP";

	static constexpr uint32_t LevelupThresholds[MaxLevel - 1] = { 200, 400, 1000, 2000 };
};


#endif //BIT_FIRMWARE_XPSYSTEM_H
