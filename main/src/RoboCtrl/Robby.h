#ifndef BIT_FIRMWARE_ROBBY_H
#define BIT_FIRMWARE_ROBBY_H

#include "RobotDriver.h"
#include "Periph/PWM.h"

namespace RoboCtrl {

class Robby : public RobotDriver {
public:
	Robby();
	virtual ~Robby();
	void hello() override;
	void setPeriod(uint32_t period); //[ms]

protected:
	void init() override;
	void deinit() override;
	void onLoop(uint micros) override;

private:
	PWM pwm;
	uint32_t period = 0;
	uint32_t t = 0;
};

}

#endif //BIT_FIRMWARE_ROBBY_H
