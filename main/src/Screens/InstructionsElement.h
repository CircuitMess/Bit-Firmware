#ifndef BIT_FIRMWARE_INSTRUCTIONSELEMENT_H
#define BIT_FIRMWARE_INSTRUCTIONSELEMENT_H


#include "LV_Interface/LVObject.h"
#include "Services/RobotManager.h"

class InstructionsElement : public LVObject {
public:
	InstructionsElement(lv_obj_t* parent, Games game);

private:
	std::string gameUIPath = "S:/Splash/";

	void buildUI();
};


#endif //BIT_FIRMWARE_INSTRUCTIONSELEMENT_H
