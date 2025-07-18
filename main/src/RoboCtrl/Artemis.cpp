#include "Artemis.h"
#include "Pins.hpp"

RoboCtrl::Artemis::Artemis() : RobotDriver(Robot::Artemis), clockPin(Pins::get(Pin::Ctrl1)), resetPin(Pins::get(Pin::Ctrl2)){
}

void RoboCtrl::Artemis::setSpeed(uint32_t tickTime){
	this->tickTime = tickTime;
}

void RoboCtrl::Artemis::init(){
	reset();
	clockPin.on();
}

void RoboCtrl::Artemis::deinit(){
	resetPin.off();
	clockPin.off();
}

void RoboCtrl::Artemis::tick(){
	clockPin.off();
	vTaskDelay(1);
	clockPin.on();
}

void RoboCtrl::Artemis::reset(){
	resetPin.on();
	vTaskDelay(1);
	resetPin.off();
}

void RoboCtrl::Artemis::onLoop(uint micros){
	timer += micros;
	if(timer >= tickTime * 1000){
		tick();
		timer = 0;
	}
}

void RoboCtrl::Artemis::hello(){

}
