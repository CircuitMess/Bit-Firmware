#ifndef BIT_FIRMWARE_MENUITEM_H
#define BIT_FIRMWARE_MENUITEM_H

#include <string>
#include <array>
#include "LV_Interface/LVObject.h"
#include "LV_Interface/LVStyle.h"

class MenuItem : public LVObject {
public:
	MenuItem(lv_obj_t* parent, const std::string& path, const std::string& pathGrayscale, bool grayedOut);
	virtual ~MenuItem();

	void setBorder(bool enabled);

	void setLocked(bool value);

private:
	class GrayscaleImageElement* img;
	lv_obj_t* overlay;

	LVStyle glow;
	lv_anim_t glowAnim;

	void startAnim();
	void stopAnim();
	static void animFunc(void* var, int32_t val);
};

#endif //BIT_FIRMWARE_MENUITEM_H
