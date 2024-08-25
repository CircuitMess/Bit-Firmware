#ifndef BIT_FIRMWARE_MRBEE_H
#define BIT_FIRMWARE_MRBEE_H

#include "RobotDriver.h"
#include "Periph/PinOut.h"
#include "LEDService/RoboLEDService.h"

namespace RoboCtrl {

class MrBee : public RobotDriver {
public:
	MrBee();
	virtual ~MrBee();
	void blink();
	void blinkContinuousFast();
	void ledOff();
	void hello() override;

protected:
	void init() override;
	void deinit() override;

private:
	RoboLEDService ledService;
};

}

#endif //BIT_FIRMWARE_MRBEE_H
