#ifndef BIT_FIRMWARE_ROBOTDRIVER_H
#define BIT_FIRMWARE_ROBOTDRIVER_H

#include "Services/Robots.h"
#include "Util/Events.h"

namespace RoboCtrl {

class RobotDriver {
public:
	RobotDriver(Robot type);
	virtual ~RobotDriver() = default;
	/**
	 * Called when robot is plugged in.
	 */
	virtual void hello() = 0;
	void loop(uint32_t micros);

protected:
	/**
	 * Checks with Robots service whether the robot is inserted
	 * This needs to be checked before any robot HW control!
	 * @return True - robot is inserted, False - robot isn't inserted
	 */


	virtual void init() = 0;
	virtual void deinit() = 0;
	bool isInited() const;

	virtual void onLoop(uint micros);

private:
	const Robot type;

	bool inserted = false;
	bool inited = false;

	static constexpr uint32_t CheckInterval = 200000; //200ms
	uint32_t checkCounter = CheckInterval; //used to trigger a check on the first loop()

	bool checkRobot() const;
};

}


#endif //BIT_FIRMWARE_ROBOTDRIVER_H
