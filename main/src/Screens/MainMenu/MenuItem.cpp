#include <cmath>
#include "MenuItem.h"
#include "Filepaths.hpp"
#include "Screens/GrayscaleImageElement.h"
#include "Util/stdafx.h"

MenuItem::MenuItem(lv_obj_t* parent, const std::string& path, const std::string& pathGrayscale, bool grayedOut) : LVObject(parent){
	lv_obj_set_size(*this, 38, 38);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_obj_add_flag(*this, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_style_set_shadow_color(glow, lv_color_white());
	lv_style_set_shadow_spread(glow, 1);
	lv_style_set_shadow_width(glow, 6);
	lv_obj_add_style(*this, glow, LV_STATE_FOCUSED);

	img = new GrayscaleImageElement(*this, path, pathGrayscale, grayedOut);

	lv_obj_set_style_border_color(*this, lv_color_white(), LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(*this, LV_OPA_TRANSP, LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(*this, LV_OPA_COVER, LV_STATE_FOCUSED);
	lv_obj_set_style_border_width(*this, 1, LV_STATE_DEFAULT);

	overlay = lv_obj_create(*this);
	lv_obj_add_flag(overlay, LV_OBJ_FLAG_FLOATING);
	lv_obj_set_pos(overlay, 0, 0);
	lv_obj_set_size(overlay, 37, 38);
	lv_obj_set_style_bg_color(overlay, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(overlay, LV_OPA_COVER, 0);
	lv_obj_move_foreground(overlay);
	lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);

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
	lv_anim_set_values(&glowAnim, 0, 255);
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
	lv_obj_set_style_shadow_opa(*item, std::round(map((float) val, 0, 255, LV_OPA_100, LV_OPA_40)), LV_STATE_FOCUSED);
	lv_obj_set_style_opa(item->overlay, std::round(map((float) val, 0, 255, LV_OPA_10, LV_OPA_20)), 0);
}

void MenuItem::setBorder(bool enabled){
	if(enabled){
		startAnim();
		lv_obj_clear_flag(overlay, LV_OBJ_FLAG_HIDDEN);
	}else{
		stopAnim();
		lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
	}
}

void MenuItem::setLocked(bool value){
	if(img == nullptr){
		return;
	}

	img->setGrayscale(value);
}
