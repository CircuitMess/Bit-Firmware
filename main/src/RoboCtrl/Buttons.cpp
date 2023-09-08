#include "Buttons.h"

RoboCtrl::Buttons::Buttons() : RobotDriver(Robot::Buttons), pwm(CTRL_1, LEDC_CHANNEL_2), chirpSystem(pwm){

}

RoboCtrl::Buttons::~Buttons(){
	chirpSystem.stop();
}


void RoboCtrl::Buttons::hello(){

}

void RoboCtrl::Buttons::init(){

}

void RoboCtrl::Buttons::deinit(){
	chirpSystem.stop();
}

void RoboCtrl::Buttons::play(const Sound& sound){
	chirpSystem.play(sound);
}
