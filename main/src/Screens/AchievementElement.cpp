#include <string>
#include "AchievementElement.h"
#include "Util/Services.h"
#include "Filepaths.hpp"

AchievementElement::AchievementElement(lv_obj_t* parent, Achievement id, Theme theme) : LVObject(parent){
	lv_obj_set_size(*this, lv_pct(100), lv_pct(100));
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

	if(!AchievementTextData.contains(id)) return;

	if(theme == Theme::COUNT){
		auto set = (Settings*) Services.get(Service::Settings);
		theme = set->get().theme;
	}

	auto achi = (AchievementSystem*) Services.get(Service::Achievements);
	const auto& data = achi->get(id);
	const auto& info = AchievementTextData.at(id);

	auto title = lv_label_create(*this);
	lv_obj_set_width(title, lv_pct(100));
	lv_label_set_text(title, info.Title);
	lv_label_set_long_mode(title, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_line_space(title, 3, 0);
	lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(title, lv_color_make(85, 126, 150), 0);

	auto icon = lv_img_create(*this);
	lv_img_set_src(icon, data.unlocked() ? AchivementFile(id) : AchivementFileBW(id));

	auto text = lv_label_create(*this);
	lv_obj_set_width(text, lv_pct(100));
	lv_label_set_text(text, info.Description);
	lv_label_set_long_mode(text, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_line_space(text, 3, 0);
	lv_obj_set_style_text_align(text, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(text, lv_color_make(85, 126, 150), 0);

	auto progress = lv_label_create(*this);
	if(data.goal == 1){
		lv_label_set_text(progress, data.unlocked() ? "UNLOCKED" : "LOCKED");
	}else{
		std::string text = std::to_string(data.progress) + "/" + std::to_string(data.goal);
		lv_label_set_text(progress, text.c_str());
	}
	lv_obj_set_width(progress, lv_pct(100));
	lv_label_set_long_mode(progress, LV_LABEL_LONG_WRAP);
	lv_obj_set_style_text_line_space(progress, 3, 0);
	lv_obj_set_style_text_align(progress, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_color(progress, lv_color_make(85, 126, 150), 0);
}
