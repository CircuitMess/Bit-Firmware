#ifndef BIT_FIRMWARE_MENUHEADER_H
#define BIT_FIRMWARE_MENUHEADER_H

#include "LV_Interface/LVObject.h"
#include "Screens/XPBar.h"

class MenuHeader : public LVObject {
public:
	MenuHeader(lv_obj_t* parent);

private:
	lv_obj_t* label;
	XPBar bar = XPBar(XPBarLength::Long, *this);

	static constexpr uint16_t Width = 128;
	static constexpr uint16_t Height = 13;

	static constexpr const char* Background = "S:/MenuHeader.bin";
	static constexpr uint32_t LVLTextColor = 0x4b5f6b;
};


#endif //BIT_FIRMWARE_MENUHEADER_H
