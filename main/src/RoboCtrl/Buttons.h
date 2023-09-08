#ifndef BIT_FIRMWARE_BUTTONS_H
#define BIT_FIRMWARE_BUTTONS_H

#include "RobotDriver.h"
#include "Services/ChirpSystem.h"

namespace RoboCtrl{

class Buttons : public RobotDriver {
public:
	Buttons();
	virtual ~Buttons();
	void hello() override;

	void play(const Sound& sound);

protected:
	void init() override;
	void deinit() override;

private:
	PWM pwm;
	ChirpSystem chirpSystem;
};

}

#endif //BIT_FIRMWARE_BUTTONS_H
