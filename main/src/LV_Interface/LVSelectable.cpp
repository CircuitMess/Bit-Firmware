#include "LVSelectable.h"
#include "InputLVGL.h"

LVSelectable::LVSelectable(lv_obj_t* parent) : LVObject(parent){
	inputGroup = lv_group_create();

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		LVSelectable* sel = static_cast<LVSelectable*>(e->user_data);
		sel->select();
	}, LV_EVENT_CLICKED, this);
}

LVSelectable::~LVSelectable(){
	lv_group_del(inputGroup);
}

void LVSelectable::select(){
	active = true;

	if(InputLVGL::getInstance()->getIndev()->group != inputGroup){
		parentGroup = InputLVGL::getInstance()->getIndev()->group;
		lv_indev_set_group(InputLVGL::getInstance()->getIndev(), inputGroup);
	}

	onSelect();
}

void LVSelectable::deselect(){
	if(parentGroup != nullptr){
		lv_indev_set_group(InputLVGL::getInstance()->getIndev(), parentGroup);
	}

	parentGroup = nullptr;
	active = false;

	lv_event_send(obj, LV_EVENT_READY, nullptr);

	onDeselect();
}

bool LVSelectable::isActive() const{
	return active;
}
