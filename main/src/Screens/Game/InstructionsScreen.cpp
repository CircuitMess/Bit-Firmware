#include "InstructionsScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "GameMenuScreen.h"
#include "Periph/NVSFlash.h"
#include "Services/RobotManager.h"
#include "Settings/Settings.h"
#include "Filepaths.hpp"

InstructionsScreen::InstructionsScreen(Games current) : evts(6), currentGame(current){
	Input* input = (Input*) Services.get(Service::Input);
	if(input == nullptr){
		return;
	}

	if(input->isPressed(Input::Button::A) || input->isPressed(Input::Button::B) || input->isPressed(Input::Button::Menu)){
		ignoreFirstInput = true;
	}

	buildUI();
}

void InstructionsScreen::buildUI(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);

	auto bg = lv_img_create(*this);
	lv_img_set_src(bg, THEMED_FILE(Background, settings->get().theme));
	lv_obj_add_flag(bg, LV_OBJ_FLAG_FLOATING);

	// TODO this needs to be sorted after the proper images for instructions get created
	/*auto top = lv_obj_create(*this);
	lv_obj_set_size(top, 128, 32);
	lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(top, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_ver(top, 4, 0);
	lv_obj_set_style_pad_right(top, 2, 0);
	lv_obj_set_style_pad_left(top, 6, 0);

	auto img = lv_img_create(top);
	lv_img_set_src(img, "S:/Paused.bin"); // TODO this needs to be changed depending on the game launched

	batt = new BatteryElement(top);

	auto rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 128, 96);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_all(rest, 4, 0);

	const std::string instrPath = "S:/Splash/" + std::string(GameIcons[(uint8_t) currentGame]) + "_instr.bmp";

	instructionsImg = lv_img_create(*this);
	lv_img_set_src(instructionsImg, instrPath.c_str());

	lv_group_focus_obj(lv_obj_get_child(*this, 0));*/
}

void InstructionsScreen::onStart(){
	Events::listen(Facility::Input, &evts);
	InputLVGL::getInstance()->setVertNav(true);
}

void InstructionsScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);
}

void InstructionsScreen::loop(){
	//batt->loop();

	for(Event e{}; evts.get(e, 0); ){
		if(e.facility != Facility::Input){
			free(e.data);
			continue;
		}

		if(ignoreFirstInput){
			ignoreFirstInput = false;
			free(e.data);
			continue;
		}

		auto data = (Input::Data*) e.data;
		if((data->btn == Input::Menu || data->btn == Input::B || data->btn == Input::A) && data->action == Input::Data::Release){
			exit();

			free(e.data);
			return;
		}

		free(e.data);
	}
}

void InstructionsScreen::exit(){
	auto ui = (UIThread*) Services.get(Service::UI);
	ui->startScreen([this](){ return std::make_unique<GameMenuScreen>(currentGame); });
}
