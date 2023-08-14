#include "MenuItem.h"
#include <cstdio>

MenuItem::MenuItem(lv_obj_t* parent, const char* path) : LVObject(parent){
	lv_obj_set_size(*this, 42, 42);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	img = lv_img_create(*this);
	lv_obj_set_size(img, 40, 40);
	lv_img_set_src(img, path);

	border = lv_img_create(*this);
	lv_obj_set_size(border, 42, 42);
	lv_img_set_src(border, "S:/IconBorder.bin");
	lv_obj_add_flag(border, LV_OBJ_FLAG_FLOATING);

	setBorder(false);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto item = static_cast<MenuItem*>(e->user_data);
		item->setBorder(true);
	}, LV_EVENT_FOCUSED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto item = static_cast<MenuItem*>(e->user_data);
		item->setBorder(false);
	}, LV_EVENT_DEFOCUSED, this);
}

void MenuItem::setBorder(bool enabled){
	if(enabled){
		lv_obj_clear_flag(border, LV_OBJ_FLAG_HIDDEN);
	}else{
		lv_obj_add_flag(border, LV_OBJ_FLAG_HIDDEN);
	}
}

void MenuItem::setIcon(const char* path){
	lv_img_set_src(img, path);
}
