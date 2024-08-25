#include "AchievementModal.h"
#include <sstream>
#include "Filepaths.hpp"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Screens/XPBar.h"

AchievementModal::AchievementModal(LVScreen* parent, AchievementData data): LVModal(parent){
	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (AchievementModal*) e->user_data;
		delete modal;
	}, LV_EVENT_PRESSED, this);

	buildUI(data);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void AchievementModal::buildUI(AchievementData data){
	Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	const Theme theme = settings->get().theme;

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	lv_obj_t* nameObj = lv_label_create(*this);
	lv_obj_set_width(nameObj, 82);
	lv_obj_set_style_text_align(nameObj, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(nameObj, THEMED_COLOR(Foreground, theme), 0);
	lv_label_set_text(nameObj, AchievementTextData.at(data.achievementID).Title);

	lv_obj_t* descObj = lv_label_create(*this);
	lv_obj_set_width(descObj, 82);
	lv_obj_set_style_text_align(descObj, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(descObj, THEMED_COLOR(Foreground, theme), 0);
	lv_label_set_text(descObj, AchievementTextData.at(data.achievementID).Description);

	std::stringstream ss;
	ss << data.progress << "/" << data.goal;

	lv_obj_t* progressObj = lv_label_create(*this);
	lv_obj_set_width(progressObj, 82);
	lv_obj_set_style_text_align(progressObj, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(progressObj, THEMED_COLOR(Foreground, theme), 0);
	lv_label_set_text(progressObj, ss.str().c_str());

	auto barBackground = lv_img_create(*this);
	lv_img_set_src(barBackground, "S:/Profile/barOutline.bin");

	XPBar* bar = new XPBar(XPBarLength::Short, barBackground, 1.0f * data.progress / data.goal);
	lv_obj_set_align(*bar, LV_ALIGN_TOP_LEFT);
	lv_obj_set_pos(*bar, 1, 1);
}
