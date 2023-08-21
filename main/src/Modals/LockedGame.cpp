#include "LockedGame.h"
#include "Services/GameManager.h"
#include <string>

LockedGame::LockedGame(LVScreen* parent, Robot rob) : LVModal(parent), rob(rob){
	buildUI();

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (LockedGame*) e->user_data;
		delete modal;
	}, LV_EVENT_CLICKED, this);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void LockedGame::buildUI(){
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	auto icon = lv_img_create(*this);
	lv_obj_set_size(icon, 22, 29);
	lv_img_set_src(icon, "S:/Lock.bin");

	std::string text("Insert ");
	text += RobotNames[rob];
	text += " to unlock this game.";

	auto label = lv_label_create(*this);
	lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
	lv_label_set_text(label, text.c_str());
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_line_space(label, 3, 0);
	lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
}
