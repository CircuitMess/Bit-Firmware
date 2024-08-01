#include <cmath>
#include "SliderElement.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

SliderElement::SliderElement(lv_obj_t* parent, const char* name, std::function<void(uint8_t)> cb, uint8_t value, float multiplier) : LVObject(parent),
																												   cb(std::move(cb)), multi(multiplier){
	auto theme = ((Settings*) Services.get(Service::Settings))->get().theme;

	lv_style_set_width(defaultStyle, lv_pct(100));
	lv_style_set_height(defaultStyle, 17);
	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_color(defaultStyle, THEMED_COLOR(HighlightPrimary, theme));
	lv_style_set_border_opa(defaultStyle, LV_OPA_COVER);
	lv_style_set_radius(defaultStyle, 2);
	lv_style_set_pad_all(defaultStyle, 3);

	lv_style_set_radius(sliderMainStyle, LV_RADIUS_CIRCLE);
	lv_style_set_border_width(sliderMainStyle, 1);
	lv_style_set_border_color(sliderMainStyle, THEMED_COLOR(HighlightPrimary, theme));


	// lv_obj_set_height(*this, Height);
	// lv_obj_set_width(*this, lv_pct(100));

	lv_obj_add_style(*this, defaultStyle, sel);

	label = lv_label_create(*this);
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(label, labelStyle, 0);
	lv_obj_set_style_text_color(label, THEMED_COLOR(PausedForeground, theme), 0);
	lv_label_set_text(label, name);

	slider = lv_slider_create(*this);
	lv_obj_remove_style_all(slider);
	lv_obj_align(slider, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(slider, SliderWidth, SliderHeight);

	lv_slider_set_range(slider, 0, std::round(100.0f / multi));
	lv_obj_set_style_pad_hor(slider, 5, 0);
	lv_obj_add_style(slider, sliderMainStyle, LV_PART_MAIN);

	lv_style_set_bg_opa(sliderKnobStyle, LV_OPA_100);
	lv_style_set_bg_color(sliderKnobStyle, THEMED_COLOR(HighlightSecondary, theme));
	lv_style_set_radius(sliderKnobStyle, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(sliderKnobStyle, -1);
	lv_style_set_pad_left(sliderKnobStyle, 0);
	lv_obj_add_style(slider, sliderKnobStyle, LV_PART_KNOB);
	lv_obj_set_style_bg_color(slider, THEMED_COLOR(HighlightSecondary, theme), LV_PART_KNOB | LV_STATE_EDITED);

	lv_obj_add_event_cb(slider, [](lv_event_t* e){
		auto el = static_cast<SliderElement*>(e->user_data);
		if(el->cb) el->cb(el->getValue());
	}, LV_EVENT_VALUE_CHANGED, this);

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		auto element = static_cast<SliderElement*>(e->user_data);
		auto key = *((uint32_t*) e->param);
		if(key != LV_KEY_UP && key != LV_KEY_DOWN) return;
		uint32_t sendKey = key == LV_KEY_UP ? LV_KEY_RIGHT : LV_KEY_LEFT;
		lv_event_send(element->slider, LV_EVENT_KEY, &sendKey);
	}, LV_EVENT_KEY, this);

	setValue(value);
}

void SliderElement::setValue(uint8_t val){
	val = std::round((float) val / multi);
	lv_slider_set_value(slider, val, LV_ANIM_OFF);
}

uint8_t SliderElement::getValue() const{
	float val = lv_slider_get_value(slider);
	return std::clamp(std::round(val * multi), 0.0f, 100.0f);
}
