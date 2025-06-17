#ifndef BIT_FIRMWARE_BATTERY_H
#define BIT_FIRMWARE_BATTERY_H

#include <cstdint>

class Battery {
public:
	enum Level { Critical = 0, VeryLow, Low, Mid, High, VeryHigh, Full, COUNT };
	enum class ChargingState : uint8_t { Unplugged, Charging, Full };

	struct Event {
		enum { Charging, LevelChange } action;
		union {
			Level level;
			ChargingState chargingState;
		};
	};

	virtual void begin(){};

	virtual bool isShutdown() const = 0;

	virtual uint8_t getPerc() const = 0;
	virtual Level getLevel() const = 0;
	virtual ChargingState getChargingState() const = 0;
};

#endif //BIT_FIRMWARE_BATTERY_H
