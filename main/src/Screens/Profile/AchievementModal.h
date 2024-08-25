#ifndef BIT_FIRMWARE_ACHIEVEMENTMODAL_H
#define BIT_FIRMWARE_ACHIEVEMENTMODAL_H

#include "LV_Interface/LVModal.h"
#include "Services/AchievementSystem.h"

class AchievementModal : public LVModal {
public:
	AchievementModal(LVScreen* parent, Achievement achi);

};


#endif //BIT_FIRMWARE_ACHIEVEMENTMODAL_H
