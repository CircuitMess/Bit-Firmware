#ifndef CLOCKSTAR_FIRMWARE_SLIDERELEMENT_H
#define CLOCKSTAR_FIRMWARE_SLIDERELEMENT_H


#include <functional>
#include "LV_Interface/LVSelectable.h"
#include "LV_Interface/LVStyle.h"

class SliderElement : public LVObject {
public:
	explicit SliderElement(lv_obj_t* parent, const char* name, std::function<void(uint8_t)> cb, uint8_t value = 0, float multiplier = 10);

	void setValue(uint8_t value); //0-100
	[[nodiscard]] uint8_t getValue() const; //0-100

private:
	lv_obj_t* label;
	lv_obj_t* slider;

	static constexpr lv_style_selector_t sel = LV_PART_MAIN | LV_STATE_DEFAULT;
	LVStyle defaultStyle;
	LVStyle labelStyle;
	LVStyle sliderMainStyle;
	LVStyle sliderKnobStyle;

	std::function<void(uint8_t)> cb;
	float multi;

	static constexpr uint8_t SliderWidth = 45;
	static constexpr uint8_t SliderHeight = 9;
	static constexpr uint8_t SliderRange = 50;
	static constexpr uint8_t MaxValue = 100;
	static constexpr uint8_t Height = 23;

};


#endif //CLOCKSTAR_FIRMWARE_SLIDERELEMENT_H
