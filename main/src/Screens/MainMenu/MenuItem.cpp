#include "MenuItem.h"
#include "Filepaths.hpp"
#include "Screens/GrayscaleImageElement.h"

MenuItem::MenuItem(lv_obj_t* parent, const std::string& path, bool grayedOut) : LVObject(parent){
	lv_obj_set_size(*this, 37, 38);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_obj_add_flag(*this, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_style_set_shadow_color(glow, lv_color_white());
	lv_style_set_shadow_spread(glow, 1);
	lv_style_set_shadow_width(glow, 6);
	lv_obj_add_style(*this, glow, LV_STATE_FOCUSED);

	img = new GrayscaleImageElement(*this, path, 35, 36, grayedOut);

	border = lv_img_create(*this);
	lv_obj_set_size(border, 37, 38);
	lv_img_set_src(border, Filepath::IconBorder);
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

MenuItem::~MenuItem(){
	stopAnim();
}

void MenuItem::startAnim(){
	lv_anim_init(&glowAnim);
	lv_anim_set_exec_cb(&glowAnim, animFunc);
	lv_anim_set_var(&glowAnim, this);
	lv_anim_set_values(&glowAnim, LV_OPA_100, LV_OPA_40);
	lv_anim_set_time(&glowAnim, 1000);
	lv_anim_set_playback_time(&glowAnim, 400);
	lv_anim_set_repeat_count(&glowAnim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_early_apply(&glowAnim, true);
	lv_anim_start(&glowAnim);
}

void MenuItem::stopAnim(){
	lv_anim_del(this, animFunc);
}

void MenuItem::animFunc(void* var, int32_t val){
	auto item = (MenuItem*) var;
	lv_obj_set_style_shadow_opa(*item, val, LV_STATE_FOCUSED);
}

void MenuItem::setBorder(bool enabled){
	if(enabled){
		startAnim();
		lv_obj_clear_flag(border, LV_OBJ_FLAG_HIDDEN);
	}else{
		stopAnim();
		lv_obj_add_flag(border, LV_OBJ_FLAG_HIDDEN);
	}
}

void MenuItem::setLocked(bool value){
	if(img == nullptr){
		return;
	}

	img->setGrayscale(value);
}
