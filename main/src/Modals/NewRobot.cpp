#include "NewRobot.h"
#include "LV_Interface/LVScreen.h"
#include "Services/GameManager.h"

NewRobot::NewRobot(LVScreen* parent, Robot rob, bool isNew) : LVModal(parent), rob(rob), isNew(isNew){
	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	buildMain();

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (NewRobot*) e->user_data;
		modal->click();
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (NewRobot*) e->user_data;
		modal->click();
	}, LV_EVENT_CANCEL, this);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void NewRobot::click(){
	if(!isNew || stage > 0){
		delete this;
		return;
	}

	stage++;
	lv_obj_clean(*this);
	buildNew();
}

void NewRobot::buildMain(){
	std::string imgPath("S:/GameIcons/");
	imgPath += RobotIcons[rob];
	imgPath += ".bin";
	auto icon = lv_img_create(*this);
	lv_obj_set_size(icon, 40, 40);
	lv_img_set_src(icon, imgPath.c_str());

	std::string text(RobotNames[rob]);
	text += "\nplugged in!";
	auto label = lv_label_create(*this);
	lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
	lv_label_set_text(label, text.c_str());
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_line_space(label, 3, 0);
	lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
}

void NewRobot::buildNew(){
	auto icon = lv_img_create(*this);
	lv_obj_set_size(icon, 22, 35);
	lv_img_set_src(icon, "S:/Lock_unlocked.bin");

	auto label = lv_label_create(*this);
	lv_obj_set_size(label, lv_pct(100), LV_SIZE_CONTENT);
	lv_label_set_text(label, "GAME UNLOCKED");
	lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_line_space(label, 3, 0);
	lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
}
