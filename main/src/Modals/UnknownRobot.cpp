#include <cstdio>
#include "UnknownRobot.h"

UnknownRobot::UnknownRobot(LVScreen* parent) : LVModal(parent){
	buildUI();

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (UnknownRobot*) e->user_data;
		modal->stop();
	}, LV_EVENT_CLICKED, this);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void UnknownRobot::buildUI(){
	auto parent = lv_obj_get_parent(*this);
	lv_obj_set_width(parent, 128);
	lv_obj_set_style_bg_img_src(parent, "S:/ModalBg2.bin", 0);
	lv_obj_set_width(*this, 112);

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_style_set_text_align(labelStyle, LV_TEXT_ALIGN_CENTER);
	lv_style_set_text_line_space(labelStyle, 3);
	lv_style_set_text_color(labelStyle, lv_color_make(85, 126, 150));

	auto label = [this](lv_obj_t* parent, const char* text){
		auto el = lv_label_create(parent);
		lv_obj_set_size(el, lv_pct(100), LV_SIZE_CONTENT);
		lv_label_set_long_mode(el, LV_LABEL_LONG_WRAP);
		lv_obj_add_style(el, labelStyle, 0);
		lv_label_set_text(el, text);
		return el;
	};

	label(*this, "NEW GAME UNLOCKED!");

	auto container = lv_obj_create(*this);
	lv_obj_set_size(container, lv_pct(100), LV_SIZE_CONTENT);
	lv_obj_set_layout(container, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_gap(container, 3, 0);

	label(container, "Go to");
	auto link = label(container, " code.circuitmess.com ");
	label(container, "to update firmware.");

	lv_obj_add_flag(*this, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_obj_add_flag(container, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
	lv_obj_set_size(link, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	lv_label_set_long_mode(link, LV_LABEL_LONG_CLIP);
}
