#include "XPBar.h"
#include "Settings/Settings.h"
#include "Filepaths.hpp"
#include <Util/Services.h>

XPBar::XPBar(XPBarLength length, lv_obj_t* parent, float fillPercent) : LVObject(parent), length(length){
	buildUI();
	setFill(fillPercent, false);
}

void XPBar::buildUI(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);

	auto index = (uint8_t) length;

	bar = lv_bar_create(*this);
	lv_obj_set_size(bar, Widths[index], Height);
	lv_obj_set_style_bg_opa(bar, LV_OPA_TRANSP, LV_PART_MAIN);
	lv_obj_set_style_bg_img_src(bar, (length == XPBarLength::Long) ? THEMED_FILE(BarLong, settings->get().theme) : (length == XPBarLength::Short) ? THEMED_FILE(BarShort, settings->get().theme) : THEMED_FILE(BarMedium, settings->get().theme),
								LV_PART_INDICATOR);
	lv_obj_set_style_anim_time(bar, AnimSpeed, LV_PART_MAIN);

	lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
}

void XPBar::setFill(float percent, bool anim){
	lv_bar_set_value(bar, percent * 100.0f, anim ? LV_ANIM_ON : LV_ANIM_OFF);
}
