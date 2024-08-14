#ifndef BIT_FIRMWARE_ACHIEVEMENTVIEW_H
#define BIT_FIRMWARE_ACHIEVEMENTVIEW_H

#include "LV_Interface/LVSelectable.h"
#include "LV_Interface/LVStyle.h"
#include <functional>

class AchievementView : public LVSelectable {
public:
	explicit AchievementView(lv_obj_t* parent);
	void setReturnFunc(std::function<void()> returnFunc);

protected:
	void onSelect() override;
	void onDeselect() override;

private:
	static constexpr const uint8_t RowWidth = 3; //4 elements wide grid

	LVStyle defaultStyle;

	std::function<void()> returnFunc;

	void initStyles();
	void buildUI();
};


#endif //BIT_FIRMWARE_ACHIEVEMENTVIEW_H
