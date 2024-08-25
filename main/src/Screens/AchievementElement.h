#ifndef BIT_FIRMWARE_ACHIEVEMENTELEMENT_H
#define BIT_FIRMWARE_ACHIEVEMENTELEMENT_H

#include "Services/AchievementSystem.h"
#include "LV_Interface/LVObject.h"
#include "Settings/Settings.h"

class AchievementElement : public LVObject {
public:
	AchievementElement(lv_obj_t* parent, Achievement id, Theme theme = Theme::COUNT);

};


#endif //BIT_FIRMWARE_ACHIEVEMENTELEMENT_H
