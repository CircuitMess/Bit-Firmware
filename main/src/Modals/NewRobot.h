#ifndef BIT_FIRMWARE_NEWROBOT_H
#define BIT_FIRMWARE_NEWROBOT_H

#include "LV_Interface/LVModal.h"
#include "Services/Robots.h"

class NewRobot : public LVModal {
public:
	NewRobot(LVScreen* parent, Robot rob, bool isNew);

private:
	const Robot rob;
	const bool isNew;

	void buildMain();
	void buildNew();

	int stage = 0;
	void click();

};


#endif //BIT_FIRMWARE_NEWROBOT_H
