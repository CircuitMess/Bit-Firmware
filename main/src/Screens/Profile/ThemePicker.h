#ifndef BIT_FIRMWARE_THEMEPICKER_H
#define BIT_FIRMWARE_THEMEPICKER_H

#include "LV_Interface/LVSelectable.h"
#include "LV_Interface/LVStyle.h"
#include "Settings/Settings.h"
#include "Services/RobotManager.h"

class ThemePicker : public LVSelectable {
public:
	explicit ThemePicker(lv_obj_t* parent);
	Theme getSelected() const;

protected:
	void onDeselect() override;
	void onSelect() override;

private:
	LVStyle focusedStyle;
	LVStyle defaultStyle;

	Settings& settings;
	RobotManager& robotManager;

	Theme currentTheme;

	static constexpr uint8_t RowWidth = 2;
};


#endif //BIT_FIRMWARE_THEMEPICKER_H
