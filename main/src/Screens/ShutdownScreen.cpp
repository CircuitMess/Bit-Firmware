#include "ShutdownScreen.h"
#include "Util/stdafx.h"
#include "Filepaths.hpp"
#include "Settings/Settings.h"
#include "Util/Services.h"

ShutdownScreen::ShutdownScreen(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	if(settings->get().theme == Theme::Theme1){
		bg = new LVGIF(*this, "S:/bg");
		lv_obj_add_flag(*bg, LV_OBJ_FLAG_FLOATING);
	}else{
		auto img = lv_img_create(*this);
		lv_img_set_src(img, THEMED_FILE(Background, settings->get().theme));
		lv_obj_add_flag(img, LV_OBJ_FLAG_FLOATING);
	}

	img = lv_img_create(*this);
	lv_img_set_src(img, Filepath::Battery::EmptyBig);
	lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);

	label = lv_label_create(*this);
	lv_label_set_text(label, "Battery critical,\nshutting down");
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);
}

void ShutdownScreen::onStart(){
	blinkTime = millis();
	startTime = millis();

	if(bg != nullptr){
		bg->start();
	}
}

void ShutdownScreen::onStop(){
	if(bg != nullptr){
		bg->stop();
	}
}

void ShutdownScreen::loop(){
	if(false && millis() - blinkTime >= BlinkPeriod){
		if(lv_obj_has_flag(img, LV_OBJ_FLAG_HIDDEN)){
			lv_obj_clear_flag(img, LV_OBJ_FLAG_HIDDEN);
		}else{
			lv_obj_add_flag(img, LV_OBJ_FLAG_HIDDEN);
		}
		blinkTime = millis();
	}

	if(millis() - startTime >= ShutdownTime){
		off();
	}
}

void ShutdownScreen::off(){
	extern void shutdown();
	shutdown();
}
