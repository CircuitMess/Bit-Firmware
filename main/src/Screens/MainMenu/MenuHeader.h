#ifndef BIT_FIRMWARE_MENUHEADER_H
#define BIT_FIRMWARE_MENUHEADER_H

#include "LV_Interface/LVObject.h"
#include "Screens/XPBar.h"
#include "LV_Interface/LVStyle.h"
#include "Screens/BatteryElement.h"

class MenuHeader : public LVObject {
public:
	MenuHeader(lv_obj_t* parent);

private:
	lv_obj_t* label;
	XPBar bar = XPBar(XPBarLength::Medium, *this);

	static constexpr uint16_t Width = 106;
	static constexpr uint16_t Height = 13;

	static constexpr const char* Background = "S:/MenuHeader.bin";

	lv_anim_t glowAnim;

	void startAnim();
	void stopAnim();

	static void animFunc(void* var, int32_t val);
};

#endif //BIT_FIRMWARE_MENUHEADER_H
