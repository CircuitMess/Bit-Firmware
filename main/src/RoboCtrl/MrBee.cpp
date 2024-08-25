#include "MrBee.h"
#include "Util/Services.h"
#include "Devices/SingleDigitalLED.h"

RoboCtrl::MrBee::MrBee() : RobotDriver(Robot::MrBee){

}

RoboCtrl::MrBee::~MrBee(){
	ledService.remove(LED::RobotCtrl1);
}

void RoboCtrl::MrBee::blink(){
	ledService.blink(LED::RobotCtrl1, 1, 200);
}

void RoboCtrl::MrBee::hello(){

}

void RoboCtrl::MrBee::init(){
	ledService.add<SingleDigitalLED>(LED::RobotCtrl1, CTRL_1);
}

void RoboCtrl::MrBee::deinit(){
	ledService.remove(LED::RobotCtrl1);
}

void RoboCtrl::MrBee::blinkContinuousFast(){
	ledService.blink(LED::RobotCtrl1, 0, 200);

}

void RoboCtrl::MrBee::ledOff(){
	ledService.off(LED::RobotCtrl1);
}
