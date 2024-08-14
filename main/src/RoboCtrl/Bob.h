#ifndef BIT_FIRMWARE_BOB_H
#define BIT_FIRMWARE_BOB_H

#include "RobotDriver.h"

class LEDService;

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
	LEDService* ledService;
};

}

#endif //BIT_FIRMWARE_BOB_H
