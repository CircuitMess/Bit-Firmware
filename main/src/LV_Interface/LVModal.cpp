#include "LVModal.h"
#include "LVScreen.h"
#include "InputLVGL.h"
#include "Filepaths.hpp"
#include "Settings/Settings.h"
#include "Util/Services.h"

LVModal* LVModal::current = nullptr;

LVModal::LVModal(LVScreen* parent) : LVObject((lv_obj_t*) *parent), parentScreen(parent){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	delete current;
	current = this;

	inputGroup = lv_group_create();

	container = lv_obj_create(*parent);
	lv_obj_set_parent(*this, container);

	lv_obj_add_event_cb(obj, [](lv_event_t* event){
		auto container = (lv_obj_t*) event->user_data;
		lv_obj_del_async(container);
	}, LV_EVENT_DELETE, container);

	lv_obj_set_size(container, 102, 92);
	lv_obj_set_style_pad_all(container, 8, 0);
	lv_obj_set_style_pad_left(container, 5, 0);
	lv_obj_set_style_pad_bottom(container, 5, 0);
	lv_obj_set_style_bg_img_src(container, THEMED_FILE(Popup, settings->get().theme), 0);
	lv_obj_set_style_bg_img_opa(container, LV_OPA_COVER, 0);

	lv_obj_set_size(*this, 86, 76);
	lv_obj_set_align(*this, LV_ALIGN_CENTER);

	lv_obj_set_align(container, LV_ALIGN_CENTER);
	lv_obj_add_flag(container, LV_OBJ_FLAG_FLOATING);

	oldGroup = InputLVGL::getInstance()->getIndev()->group;
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), inputGroup);
}

LVModal::~LVModal(){
	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), oldGroup);
	lv_group_del(inputGroup);
	current = nullptr;
}

void LVModal::setBg(const char* src){
	lv_obj_set_style_bg_img_src(container, src, 0);
}
