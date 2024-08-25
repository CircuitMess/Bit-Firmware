#ifndef BIT_FIRMWARE_ACHIEVEMENTELEMENT_H
#define BIT_FIRMWARE_ACHIEVEMENTELEMENT_H

#include "Services/AchievementSystem.h"
#include "LV_Interface/LVObject.h"
#include "Settings/Settings.h"
#include "LV_Interface/LVStyle.h"

class AchievementElement : public LVObject {
public:
	AchievementElement(lv_obj_t* parent, Achievement id, Theme theme = Theme::COUNT);

private:
	LVStyle textStyle;

};


#endif //BIT_FIRMWARE_ACHIEVEMENTELEMENT_H
