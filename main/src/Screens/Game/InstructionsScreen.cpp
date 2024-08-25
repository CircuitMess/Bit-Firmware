#include "InstructionsScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "GameMenuScreen.h"
#include "Periph/NVSFlash.h"
#include "Services/RobotManager.h"
#include "Services/LEDService.h"

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
	if(!launch){
		Events::listen(Facility::Input, &evts);
		InputLVGL::getInstance()->setVertNav(true);
	}

	auto led = (LEDService*) Services.get(Service::LED);
	led->ctrls(currentGame);
}

void InstructionsScreen::onStop(){
	Events::unlisten(&evts);
	InputLVGL::getInstance()->setVertNav(false);

	if(!launch){
		auto led = (LEDService*) Services.get(Service::LED);
		led->twinkle();
	}
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
