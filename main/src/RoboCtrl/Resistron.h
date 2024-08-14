#ifndef BIT_FIRMWARE_RESISTRON_H
#define BIT_FIRMWARE_RESISTRON_H

#include "RobotDriver.h"

class LEDService;

namespace RoboCtrl {

class Resistron : public RobotDriver {
public:
	Resistron();
	virtual ~Resistron();
	void blink();
	void blinkTwice();
	void blinkContinuous();

	void hello() override;
protected:
	void init() override;
	void deinit() override;
	void onLoop(uint micros) override;

private:
	LEDService* ledService;

	bool leftRightAnim = false;
	uint32_t leftRightTime = 0;
	uint32_t timer = 0;
	bool ledIndex = false;
	uint32_t repeatCounter = 0;
	uint32_t repeatNum = 0;

	static constexpr uint32_t BlinkTime = 100;

};

}

#endif //BIT_FIRMWARE_RESISTRON_H
