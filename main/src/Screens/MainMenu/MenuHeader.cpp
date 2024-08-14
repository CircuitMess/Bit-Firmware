#include "MenuHeader.h"
#include "LV_Interface/Theme/theme.h"
#include "Services/XPSystem.h"
#include "Util/Services.h"
#include "Settings/Settings.h"
#include "Filepaths.hpp"

MenuHeader::MenuHeader(lv_obj_t* parent) : LVObject(parent){
	const Settings* settings = (Settings*) Services.get(Service::Settings);

	lv_obj_set_size(obj, Width, Height);
	lv_obj_set_style_bg_img_src(obj, THEMED_FILE(Header, settings->get().theme), 0);

	lv_obj_add_flag(*this, LV_OBJ_FLAG_OVERFLOW_VISIBLE);

	lv_obj_set_style_border_width(*this, 2, 0);
	lv_obj_set_style_border_color(*this, THEMED_COLOR(Foreground, settings->get().theme), 0);
	lv_obj_set_style_border_opa(*this, LV_OPA_TRANSP, 0);

	lv_obj_set_pos(bar, 3, 2);

	label = lv_label_create(*this);
	lv_obj_set_pos(label, 78, 2);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_size(label, 27, 7);
	lv_obj_set_style_text_font(label, &landerfont, 0);
	lv_obj_set_style_text_color(label, THEMED_COLOR(Foreground, settings->get().theme), 0);

	auto* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	auto lvl = xpSystem->getLevel();
	auto percent = xpSystem->MapXPToLevel(xpSystem->getXP()).progress;
	bar.setFill(percent);
	lv_label_set_text_fmt(label, "LVL %d", lvl);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto menuHeader = static_cast<MenuHeader*>(e->user_data);
		menuHeader->startAnim();
	}, LV_EVENT_FOCUSED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto menuHeader = static_cast<MenuHeader*>(e->user_data);
		menuHeader->stopAnim();
	}, LV_EVENT_DEFOCUSED, this);

}

void MenuHeader::startAnim(){
	lv_anim_init(&glowAnim);
	lv_anim_set_exec_cb(&glowAnim, animFunc);
	lv_anim_set_var(&glowAnim, this);
	lv_anim_set_values(&glowAnim, LV_OPA_100, LV_OPA_0);
	lv_anim_set_time(&glowAnim, 1000);
	lv_anim_set_playback_time(&glowAnim, 400);
	lv_anim_set_repeat_count(&glowAnim, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_early_apply(&glowAnim, true);
	lv_anim_start(&glowAnim);
}

void MenuHeader::stopAnim(){
	lv_anim_del(this, animFunc);
}

void MenuHeader::animFunc(void* var, int32_t val){
	auto item = (MenuHeader*) var;
	lv_obj_set_style_border_opa(*item, val, LV_STATE_FOCUSED);
}
