#include "BatteryElement.h"
#include "Util/stdafx.h"
#include "Util/Services.h"
#include "Settings/Settings.h"

BatteryElement::BatteryElement(lv_obj_t* parent) : LVObject(parent), evts(6){
	auto theme = ((Settings*) Services.get(Service::Settings))->get().theme;
	BatteryIcons[0] = THEMED_FILE(Battery::Level1, theme);
	BatteryIcons[1] = THEMED_FILE(Battery::Level2, theme);
	BatteryIcons[2] = THEMED_FILE(Battery::Level3, theme);
	BatteryIcons[3] = THEMED_FILE(Battery::Level4, theme);
	BatteryIcons[4] = THEMED_FILE(Battery::Level5, theme);
	BatteryIcons[5] = THEMED_FILE(Battery::Level6, theme);

	lv_obj_set_size(*this, Width, Height);
	lv_obj_set_style_bg_img_src(*this, THEMED_FILE(Battery::Background, theme), LV_STATE_DEFAULT);
	lv_obj_set_style_pad_all(*this, 3, LV_STATE_DEFAULT);


	img = lv_img_create(*this);
	auto batt = (Battery*) Services.get(Service::Battery);
	set(batt->getLevel());
	Events::listen(Facility::Battery, &evts);
}

BatteryElement::~BatteryElement(){
	Events::unlisten(&evts);
}

void BatteryElement::set(Battery::Level battLevel){
	if(battLevel == Battery::Critical){
		battLevel = Battery::VeryLow;
	}
	this->level = (Level) (((int) battLevel) - 1); //-1 because Critical level is ignored

	lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
	lv_img_set_src(img, BatteryIcons[level]);
	if(level == Empty){
		lowBatteryAnimToggle = true;
		lowBatMillis = millis();
	}
}

void BatteryElement::loop(){
	Event evt;
	if(evts.get(evt, 0)){
		auto data = (Battery::Event*) evt.data;
		if(data->action == Battery::Event::LevelChange){
			set(data->level);
		}
		free(evt.data);
	}

	if(level == Empty){
		if(millis() - lowBatMillis > LowBatteryAnimTime){
			lowBatMillis = millis();
			if(lowBatteryAnimToggle){
				lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
			}else{
				lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
			}
			lowBatteryAnimToggle = !lowBatteryAnimToggle;
		}
	}
}
