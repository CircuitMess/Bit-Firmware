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
#include "Services/TwinkleService.h"
#include "Util/stdafx.h"

InstructionsScreen::InstructionsScreen(Games current, bool launch) : evts(6), currentGame(current), instrElement(obj, currentGame), launch(launch){
	const Input* input = (Input*) Services.get(Service::Input);
	if(input == nullptr){
		return;
	}

	if(input->isPressed(Input::Button::A) || input->isPressed(Input::Button::B) || input->isPressed(Input::Button::Menu)){
		ignoreFirstInput = true;
	}
}

void InstructionsScreen::onStart(){
	if(auto twinkle = (TwinkleService*) Services.get(Service::Twinkle)){
		twinkle->stop();
	}
	if(auto led = (LEDService*) Services.get(Service::LED)){
		auto buttons = GameButtonsUsed[(uint8_t) currentGame];
		if(buttons.up){
			led->on(LED::Up);
		}
		if(buttons.down){
			led->on(LED::Down);
		}
		if(buttons.left){
			led->on(LED::Left);
		}
		if(buttons.right){
			led->on(LED::Right);
		}
		if(buttons.a){
			led->on(LED::A);
		}
		if(buttons.b){
			led->on(LED::B);
		}
	}

	if(!launch){
		Events::listen(Facility::Input, &evts);
		InputLVGL::getInstance()->setVertNav(true);
	}
}

void InstructionsScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);
}

void InstructionsScreen::loop(){
	if(launch){
		if(auto ui = (UIThread*) Services.get(Service::UI)){
			ui->startGame(currentGame);
		}
		return;
	}

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
