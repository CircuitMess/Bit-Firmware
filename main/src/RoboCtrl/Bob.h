#ifndef BIT_FIRMWARE_BOB_H
#define BIT_FIRMWARE_BOB_H

#include "RobotDriver.h"
#include "LEDService/RoboLEDService.h"

class RoboLEDService;

namespace RoboCtrl {

class Bob : public RobotDriver {
public:
	Bob();
	virtual ~Bob();
	void hello() override;

	void blink();
	void blinkTwice();
	void blinkContinuousSlow();
	void blinkContinuousFast();

protected:
	void init() override;
	void deinit() override;

private:
	RoboLEDService ledService;
};

}

#endif //BIT_FIRMWARE_BOB_H
