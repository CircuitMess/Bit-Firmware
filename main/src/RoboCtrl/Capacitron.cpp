#include "Capacitron.h"
#include "Pins.hpp"
#include "Devices/SingleDigitalLED.h"

RoboCtrl::Capacitron::Capacitron() : RobotDriver(Robot::Capacitron){

}

RoboCtrl::Capacitron::~Capacitron(){
	ledService.remove(LED::RobotCtrl1);
	ledService.remove(LED::RobotCtrl2);

}

void RoboCtrl::Capacitron::hello(){

}

void RoboCtrl::Capacitron::init(){
	ledService.add<SingleDigitalLED>(LED::RobotCtrl1, CTRL_1);
	ledService.add<SingleDigitalLED>(LED::RobotCtrl2, CTRL_2);

}

void RoboCtrl::Capacitron::deinit(){
	ledService.remove(LED::RobotCtrl1);
	ledService.remove(LED::RobotCtrl2);
}

void RoboCtrl::Capacitron::leftRightContinuous(uint32_t period){
	ledService.on(LED::RobotCtrl1);
	ledService.off(LED::RobotCtrl2);
	leftRightAnim = true;
	leftRightTime = period;
}

void RoboCtrl::Capacitron::allOn(){
	ledService.on(LED::RobotCtrl1);
	ledService.on(LED::RobotCtrl2);
	leftRightAnim = false;
}

void RoboCtrl::Capacitron::flashingContinuous(int32_t repeats){
	ledService.blink(LED::RobotCtrl1, repeats, 600);
	ledService.blink(LED::RobotCtrl2, repeats, 600);
	leftRightAnim = false;
}

void RoboCtrl::Capacitron::onLoop(uint micros){
	if(!leftRightAnim) return;

	timer += micros;
	if(timer >= leftRightTime * 1000){
		if(ledIndex){
			ledService.on(LED::RobotCtrl1);
			ledService.off(LED::RobotCtrl2);
		}else{
			ledService.off(LED::RobotCtrl1);
			ledService.on(LED::RobotCtrl2);
		}
		ledIndex = !ledIndex;
		timer = 0;
	}
}
