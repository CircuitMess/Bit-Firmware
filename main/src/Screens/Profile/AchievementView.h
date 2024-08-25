#ifndef BIT_FIRMWARE_ACHIEVEMENTVIEW_H
#define BIT_FIRMWARE_ACHIEVEMENTVIEW_H

#include "LV_Interface/LVSelectable.h"
#include "LV_Interface/LVStyle.h"
#include "Services/AchievementSystem.h"
#include "LV_Interface/LVScreen.h"
#include "Screens/Profile/AchievementModal.h"
#include <functional>

class AchievementView : public LVSelectable {
public:
	AchievementView(LVScreen* screen, lv_obj_t* parent, uint8_t rows, uint16_t width, uint16_t height);
	void setReturnFunc(std::function<void()> returnFunc);

protected:
	void onSelect() override;
	void onDeselect() override;

private:
	LVScreen* screen;
	AchievementModal* modal = nullptr;
	const uint8_t RowWidth;
	const uint16_t width;
	const uint16_t height;

	LVStyle defaultStyle;

	std::function<void()> returnFunc;

	std::vector<AchievementData> achievementsVector;

	void initStyles();
	void buildUI();
};


#endif //BIT_FIRMWARE_ACHIEVEMENTVIEW_H
