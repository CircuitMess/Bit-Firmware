#include "BatteryElement.h"
#include "Util/stdafx.h"
#include "Util/Services.h"

BatteryElement::BatteryElement(lv_obj_t* parent) : LVObject(parent), evts(6){
	Events::listen(Facility::Battery, &evts);
	lv_obj_set_size(*this, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
	img = lv_img_create(*this);

	auto batt = (Battery*) Services.get(Service::Battery);
	set(batt->getLevel());
}

BatteryElement::~BatteryElement(){
	Events::unlisten(&evts);
}

void BatteryElement::set(Battery::Level battLevel){
	if(battLevel == Battery::Critical){
		battLevel = Battery::VeryLow;
	}
	this->level = (Level) (((int) battLevel) - 1);

	if(level == Empty){
		lowBatteryAnimToggle = true;
		lowBatMillis = millis();
		lv_img_set_src(img, BatteryIcons[0]);
	}else if(level == Charging){
		chargingMillis = millis();
		chargingIndex = 0;
		lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
		lv_img_set_src(img, BatteryIcons[0]);
	}else{
		lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
		lv_img_set_src(img, BatteryIcons[level - 1]);
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

	if(level == Charging){
		if(millis() - chargingMillis > ChargingAnimTime){
			chargingIndex = (chargingIndex + 1) % BatteryLevels;
			chargingMillis = millis();
			lv_img_set_src(img, BatteryIcons[chargingIndex]);
		}
	}else if(level == Empty){
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
