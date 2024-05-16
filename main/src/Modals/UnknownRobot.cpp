#include <cstdio>
#include "UnknownRobot.h"
#include "Filepaths.hpp"

UnknownRobot::UnknownRobot(LVScreen* parent) : LVModal(parent){
	buildUI();

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (UnknownRobot*) e->user_data;
		delete modal;
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (UnknownRobot*) e->user_data;
		delete modal;
	}, LV_EVENT_CANCEL, this);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void UnknownRobot::buildUI(){
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	auto icon = lv_img_create(*this);
	lv_obj_set_size(icon, 22, 29);
	lv_img_set_src(icon, Filepath::Locked);

	auto label = lv_label_create(*this);
	lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
	lv_label_set_text(label, "Unknown robot!");
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_line_space(label, 3, 0);
	lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
}
