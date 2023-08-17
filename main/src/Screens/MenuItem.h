#ifndef BIT_FIRMWARE_MENUITEM_H
#define BIT_FIRMWARE_MENUITEM_H

#include "LV_Interface/LVObject.h"
#include "LV_Interface/LVStyle.h"

class MenuItem : public LVObject {
public:
	MenuItem(lv_obj_t* parent, const char* path);
	virtual ~MenuItem();

	void setBorder(bool enabled);

	void setIcon(const char* path);

private:
	lv_obj_t* img;
	lv_obj_t* border;

	LVStyle glow;
	lv_anim_t glowAnim;

	void startAnim();
	void stopAnim();
	static void animFunc(void* var, int32_t val);

};


#endif //BIT_FIRMWARE_MENUITEM_H
