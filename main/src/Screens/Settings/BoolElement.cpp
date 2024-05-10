#include "BoolElement.h"

#include <utility>
#include <cstdio>
#include "widgets/lv_switch.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

lv_style_transition_dsc_t BoolElement::Transition;

BoolElement::BoolElement(lv_obj_t* parent, const char* name, std::function<void(bool)> cb, bool value) : LVObject(parent), value(value), cb(std::move(cb)){
	lv_obj_add_flag(*this, LV_OBJ_FLAG_CLICKABLE);

	auto theme = ((Settings*) Services.get(Service::Settings))->get().theme;

	lv_style_set_width(defaultStyle, lv_pct(100));
	lv_style_set_height(defaultStyle, 17);
	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_color(defaultStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_border_opa(defaultStyle, LV_OPA_COVER);
	lv_style_set_radius(defaultStyle, 2);
	lv_style_set_pad_all(defaultStyle, 3);

	lv_style_set_bg_color(focusedStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_bg_opa(focusedStyle, LV_OPA_30);

	lv_style_set_border_width(switchStyle, 1);
	lv_style_set_border_color(switchStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_bg_opa(switchStyle, LV_OPA_0);
	lv_style_set_radius(switchStyle, LV_RADIUS_CIRCLE);
	lv_style_set_anim_time(switchStyle, 120);

	lv_style_set_border_width(switchCheckedStyle, 1);
	lv_style_set_border_color(switchCheckedStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_bg_color(switchCheckedStyle, THEMED_COLOR(HighlightSecondary, theme));
	lv_style_set_bg_opa(switchCheckedStyle, LV_OPA_COVER);
	lv_style_set_text_color(switchCheckedStyle, THEMED_COLOR(PausedForeground, theme));

	lv_style_set_bg_color(switchKnobStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_bg_opa(switchKnobStyle, LV_OPA_COVER);
	lv_style_set_radius(switchKnobStyle, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(switchKnobStyle, -1);


	// lv_obj_set_height(*this, Height);
	// lv_obj_set_width(*this, lv_pct(100));

	lv_obj_add_style(*this, focusedStyle, SelFocus);
	lv_obj_add_style(*this, defaultStyle, SelDefault);

	label = lv_label_create(*this);
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(label, labelStyle, 0);
	lv_obj_set_style_text_color(label, THEMED_COLOR(PausedForeground, theme), 0);
	lv_label_set_text(label, name);

	switchElement = lv_switch_create(*this);
	lv_obj_align(switchElement, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(switchElement, SwitchWidth, SwitchHeight);

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		auto element = static_cast<BoolElement*>(e->user_data);
		element->setValue(!element->value);
		if(element->cb) element->cb(element->value);
		lv_event_send(element->switchElement, LV_EVENT_VALUE_CHANGED, nullptr);
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		auto element = static_cast<BoolElement*>(e->user_data);
		auto key = *((uint32_t*) e->param);
		if((key == LV_KEY_UP && !element->value) || (key == LV_KEY_DOWN && element->value)){
			lv_event_send(*element, LV_EVENT_CLICKED, nullptr);
		}
	}, LV_EVENT_KEY, this);

	lv_obj_add_style(switchElement, switchStyle, 0);
	lv_obj_add_style(switchElement, switchCheckedStyle, LV_PART_INDICATOR | LV_STATE_CHECKED);
	lv_obj_add_style(switchElement, switchKnobStyle, LV_PART_KNOB);
	lv_obj_set_style_radius(switchElement, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);

	lv_style_transition_dsc_init(&Transition, TransProps, lv_anim_path_linear, 150, 0, NULL);
	lv_obj_set_style_transition(switchElement, &Transition, LV_PART_INDICATOR);
	lv_obj_set_style_transition(switchElement, &Transition, LV_PART_INDICATOR | LV_STATE_CHECKED);

	Transition.time = 0;
	setValue(value);
	Transition.time = 150;
}

void BoolElement::setValue(bool value){
	this->value = value;
	value ? lv_obj_add_state(switchElement, LV_STATE_CHECKED) : lv_obj_clear_state(switchElement, LV_STATE_CHECKED);
}

bool BoolElement::getValue() const{
	return value;
}
