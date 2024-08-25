#include "Bob.h"
#include "Pins.hpp"
#include "Devices/SingleDigitalLED.h"

RoboCtrl::Bob::Bob() : RobotDriver(Robot::Bob){
}

RoboCtrl::Bob::~Bob(){
	ledService.remove(LED::RobotCtrl1);
}

void RoboCtrl::Bob::hello(){

}

void RoboCtrl::Bob::init(){
	ledService.add<SingleDigitalLED>(LED::RobotCtrl1, CTRL_1);
}

void RoboCtrl::Bob::deinit(){
	ledService.remove(LED::RobotCtrl1);
}

void RoboCtrl::Bob::blink(){
	ledService.blink(LED::RobotCtrl1, 1, 200);
}

void RoboCtrl::Bob::blinkContinuousSlow(){
	ledService.blink(LED::RobotCtrl1, 0, 500);
}

void RoboCtrl::Bob::blinkContinuousFast(){
	ledService.blink(LED::RobotCtrl1, 0, 200);
}

void RoboCtrl::Bob::blinkTwice(){
	ledService.blink(LED::RobotCtrl1, 2, 200);
}
