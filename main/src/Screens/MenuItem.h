#ifndef BIT_FIRMWARE_MENUITEM_H
#define BIT_FIRMWARE_MENUITEM_H

#include "LV_Interface/LVObject.h"

class MenuItem : public LVObject {
public:
	MenuItem(lv_obj_t* parent, const char* path);

	void setBorder(bool enabled);

	void setIcon(const char* path);

private:
	lv_obj_t* img;
	lv_obj_t* border;

};


#endif //BIT_FIRMWARE_MENUITEM_H
