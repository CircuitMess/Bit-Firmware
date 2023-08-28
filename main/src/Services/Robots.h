#ifndef BIT_FIRMWARE_ROBOTS_H
#define BIT_FIRMWARE_ROBOTS_H

#include "Util/Threaded.h"
#include "Pins.hpp"

enum Robot : uint8_t { MrBee, Resistron, Artemis, Robby, Marv, Capacitron, Bob, Buttons, Hertz, COUNT };

class Robots : private SleepyThreaded {
public:
	Robots();

	struct Event {
		enum { Insert, Remove } action;
		Robot robot;
	};

	Robot getInserted();

private:
	static constexpr uint32_t CheckInterval = 500; // [ms]

	static constexpr uint8_t AddrPins[] = { ADDR_1, ADDR_2, ADDR_3, ADDR_4, ADDR_5, ADDR_6 };
	static constexpr uint8_t CtrlPins[] = { CTRL_1, CTRL_2, CTRL_3, CTRL_4, CTRL_5, CTRL_6 };

	bool inserted = false;
	uint8_t current = -1;

	void sleepyLoop() override;

	bool checkInserted();
	uint8_t checkAddr();

};


#endif //BIT_FIRMWARE_ROBOTS_H
