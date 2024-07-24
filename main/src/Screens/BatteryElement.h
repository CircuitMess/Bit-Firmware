#ifndef CLOCKSTAR_FIRMWARE_BATTERYELEMENT_H
#define CLOCKSTAR_FIRMWARE_BATTERYELEMENT_H


#include "LV_Interface/LVObject.h"
#include "Util/Events.h"
#include "Devices/Battery.h"
#include "Filepaths.hpp"

class BatteryElement : public LVObject {
public:
	enum Level {
		Empty, VeryLow, Low, Mid, High, Full
	};

	BatteryElement(lv_obj_t* parent);
	virtual ~BatteryElement();

	void loop();

private:
	lv_obj_t* img;
	EventQueue evts;
	Level level;

	void set(Battery::Level battLevel);

	static constexpr uint8_t Width = 20;
	static constexpr uint8_t Height = 13;

	uint8_t chargingIndex = 0;
	static constexpr uint32_t ChargingAnimTime = 500;
	static constexpr uint8_t BatteryLevels = 6;
	uint32_t chargingMillis = 0;

	static constexpr uint32_t LowBatteryAnimTime = 500;
	bool lowBatteryAnimToggle = false;
	uint32_t lowBatMillis = 0;

	const char* BatteryIcons[BatteryLevels];
};


#endif //CLOCKSTAR_FIRMWARE_BATTERYELEMENT_H
