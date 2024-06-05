#ifndef BIT_FIRMWARE_LOCKEDGAME_H
#define BIT_FIRMWARE_LOCKEDGAME_H


#include "LV_Interface/LVModal.h"
#include "Services/Robots.h"

class LockedGame : public LVModal {
public:
	LockedGame(LVScreen* parent, RobotData rob);

private:
	const RobotData rob;

	void buildUI();

};


#endif //BIT_FIRMWARE_LOCKEDGAME_H
