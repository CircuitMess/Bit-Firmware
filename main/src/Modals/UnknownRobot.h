#ifndef BIT_FIRMWARE_UNKNOWNROBOT_H
#define BIT_FIRMWARE_UNKNOWNROBOT_H

#include "LV_Interface/LVModal.h"
#include "LV_Interface/LVStyle.h"

class UnknownRobot : public LVModal {
public:
	UnknownRobot(LVScreen* parent);

private:

	void buildUI();

	LVStyle labelStyle;

};


#endif //BIT_FIRMWARE_UNKNOWNROBOT_H
