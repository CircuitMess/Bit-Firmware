#ifndef BIT_FIRMWARE_THEMEPICKER_H
#define BIT_FIRMWARE_THEMEPICKER_H

#include "LV_Interface/LVSelectable.h"
#include "LV_Interface/LVStyle.h"
#include "Settings/Settings.h"
#include "Services/RobotManager.h"

class ThemePicker : public LVSelectable {
public:
	explicit ThemePicker(lv_obj_t* parent);

protected:
	void onDeselect() override;
	void onSelect() override;

private:
	LVStyle focusedStyle;
	LVStyle defaultStyle;

	void changeTheme(Theme theme);

	Settings& settings;
	RobotManager& robotManager;

	static constexpr uint8_t RowWidth = 2;
};


#endif //BIT_FIRMWARE_THEMEPICKER_H
