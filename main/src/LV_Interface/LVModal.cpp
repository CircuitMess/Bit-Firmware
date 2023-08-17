#include "LVModal.h"
#include "LVScreen.h"
#include "InputLVGL.h"

LVModal* LVModal::current = nullptr;

LVModal::LVModal(LVScreen* parent) : LVObject((lv_obj_t*) *parent), parentScreen(parent){
	inputGroup = lv_group_create();

	container = lv_obj_create(*parent);
	lv_obj_set_parent(*this, container);

	lv_obj_add_event_cb(obj, [](lv_event_t* event){
		auto container = (lv_obj_t*) event->user_data;
		lv_obj_del_async(container);
	}, LV_EVENT_DELETE, container);

	lv_obj_set_size(container, 102, 92);
	lv_obj_set_style_pad_all(container, 8, 0);
	lv_obj_set_style_bg_img_src(container, "S:/ModalBg.bin", 0);
	lv_obj_set_style_bg_img_opa(container, LV_OPA_COVER, 0);

	lv_obj_set_size(*this, 86, 76);
	lv_obj_set_align(*this, LV_ALIGN_CENTER);

	lv_obj_set_align(container, LV_ALIGN_CENTER);
	lv_obj_add_flag(container, LV_OBJ_FLAG_FLOATING);
	lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
}

LVModal::~LVModal(){
	if(active){
		current = nullptr;
		lv_indev_set_group(InputLVGL::getInstance()->getIndev(), parentScreen->getInputGroup());
	}
	lv_group_del(inputGroup);
}

void LVModal::start(){
	if(active) return;

	if(current){
		current->stop();
	}

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), inputGroup);

	lv_obj_clear_flag(container, LV_OBJ_FLAG_HIDDEN);
	lv_obj_invalidate(container);

	active = true;
	current = this;

	onStart();
}

void LVModal::stop(){
	if(!active) return;

	lv_indev_set_group(InputLVGL::getInstance()->getIndev(), parentScreen->getInputGroup());

	lv_obj_add_flag(container, LV_OBJ_FLAG_HIDDEN);
	lv_obj_invalidate(container);

	active = false;
	current = nullptr;

	onStop();
}

bool LVModal::isActive() const{
	return active;
}

void LVModal::onStart(){

}

void LVModal::onStop(){

}
