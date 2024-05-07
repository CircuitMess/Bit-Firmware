#include "MenuHeader.h"
#include "LV_Interface/Theme/theme.h"
#include "Services/XPSystem.h"
#include "Util/Services.h"

MenuHeader::MenuHeader(lv_obj_t* parent) : LVObject(parent){
	lv_obj_set_size(obj, Width, Height);
	lv_obj_set_style_bg_img_src(obj, Background, 0);

	lv_obj_set_pos(bar, 5, 4);

	label = lv_label_create(*this);
	lv_obj_set_pos(label, 100, 3);
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_size(label, 27, 7);
	lv_obj_set_style_text_font(label, &devin, 0);
	lv_obj_set_style_text_color(label, lv_color_hex(LVLTextColor), 0);

	auto* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	auto lvl = xpSystem->getLevel();
	auto percent = xpSystem->MapXPToLevel(xpSystem->getXP()).progress;
	bar.setFill(percent);
	lv_label_set_text_fmt(label, "LVL %d", lvl);
}
