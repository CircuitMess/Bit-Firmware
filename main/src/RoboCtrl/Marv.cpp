#include "Marv.h"
#include "Pins.hpp"

RoboCtrl::Marv::Marv() : RobotDriver(Robot::Marv), clockPin(Pins::get(Pin::Ctrl1)), resetPin(Pins::get(Pin::Ctrl2)){
}

void RoboCtrl::Marv::setSpeed(uint32_t tickTime){
	this->tickTime = tickTime;
}

void RoboCtrl::Marv::init(){
	reset();
	clockPin.on();
}

void RoboCtrl::Marv::deinit(){
	resetPin.off();
	clockPin.off();
}

void RoboCtrl::Marv::tick(){
	clockPin.off();
	vTaskDelay(1);
	clockPin.on();
}

void RoboCtrl::Marv::reset(){
	resetPin.on();
	vTaskDelay(1);
	resetPin.off();
}

void RoboCtrl::Marv::onLoop(uint micros){
	timer += micros;
	if(timer >= tickTime * 1000){
		tick();
		timer = 0;
	}
}

void RoboCtrl::Marv::hello(){

}
