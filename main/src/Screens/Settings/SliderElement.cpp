#include "SliderElement.h"

SliderElement::SliderElement(lv_obj_t* parent, const char* name, std::function<void(uint8_t)> cb, uint8_t value) : LVSelectable(parent), value(value),
																												   cb(std::move(cb)){

	lv_style_set_width(defaultStyle, lv_pct(100));
	lv_style_set_height(defaultStyle, 17);
	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_color(defaultStyle, lv_color_make(217, 153, 186));
	lv_style_set_border_opa(defaultStyle, LV_OPA_COVER);
	lv_style_set_radius(defaultStyle, 2);
	lv_style_set_pad_all(defaultStyle, 3);

	lv_style_set_bg_color(focusedStyle, lv_color_make(217, 153, 186));
	lv_style_set_bg_opa(focusedStyle, LV_OPA_30);

	lv_style_set_radius(sliderMainStyle, LV_RADIUS_CIRCLE);
	lv_style_set_border_width(sliderMainStyle, 1);
	lv_style_set_border_color(sliderMainStyle, lv_color_make(217, 153, 186));


	// lv_obj_set_height(*this, Height);
	// lv_obj_set_width(*this, lv_pct(100));

	lv_obj_add_style(*this, focusedStyle, selFocus);
	lv_obj_add_style(*this, defaultStyle, sel);

	label = lv_label_create(*this);
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_add_style(label, labelStyle, 0);
	lv_label_set_text(label, name);

	slider = lv_slider_create(*this);
	lv_obj_remove_style_all(slider);
	lv_obj_align(slider, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(slider, SliderWidth, SliderHeight);

	lv_slider_set_range(slider, 0, SliderRange);
	lv_obj_set_style_pad_hor(slider, 5, 0);
	lv_obj_add_style(slider, sliderMainStyle, LV_PART_MAIN);

	lv_style_set_bg_opa(sliderKnobStyle, LV_OPA_100);
	lv_style_set_bg_color(sliderKnobStyle, lv_color_make(217, 153, 186));
	lv_style_set_radius(sliderKnobStyle, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(sliderKnobStyle, -1);
	lv_obj_add_style(slider, sliderKnobStyle, LV_PART_KNOB);
	lv_obj_set_style_bg_color(slider, lv_color_hex(0xa34578), LV_PART_KNOB | LV_STATE_EDITED);

	lv_group_add_obj(inputGroup, slider);

	lv_obj_add_event_cb(slider, [](lv_event_t* e){
		auto element = static_cast<SliderElement*>(e->user_data);
		element->deselect();
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(slider, [](lv_event_t* e){
		auto element = static_cast<SliderElement*>(e->user_data);
		element->value = lv_slider_get_value(element->slider) * MaxValue / SliderRange;
		if(element->cb) element->cb(element->value);
	}, LV_EVENT_VALUE_CHANGED, this);

	lv_obj_add_event_cb(obj, [](lv_event_t* e){
		auto element = static_cast<SliderElement*>(e->user_data);
		lv_group_set_editing(element->inputGroup, true);
	}, LV_EVENT_CLICKED, this);

	setValue(value);
}

void SliderElement::setValue(uint8_t value){
	this->value = value;
	lv_slider_set_value(slider, value * SliderRange / MaxValue, LV_ANIM_OFF);
}

uint8_t SliderElement::getValue() const{
	return value;
}