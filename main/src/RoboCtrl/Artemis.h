#ifndef BIT_FIRMWARE_ARTEMIS_H
#define BIT_FIRMWARE_ARTEMIS_H

#include "RobotDriver.h"
#include "Periph/PinOut.h"

namespace RoboCtrl {

class Artemis : public RobotDriver {
public:
	Artemis();
	void setSpeed(uint32_t tickTime); //[ms]
	void hello() override;

protected:
	void init() override;
	void deinit() override;
	void onLoop(uint micros) override;

private:
	void tick();
	void reset();

	PinOut clockPin;
	PinOut resetPin;

	uint32_t tickTime = 0;
	uint32_t timer = 0;
};

}
#endif //BIT_FIRMWARE_ARTEMIS_H
