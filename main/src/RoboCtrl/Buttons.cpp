#include "Buttons.h"

RoboCtrl::Buttons::Buttons() : RobotDriver(Robot::Buttons), pwm(Pins::get(Pin::Ctrl1), LEDC_CHANNEL_2), chirpSystem(pwm){

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

void RoboCtrl::Buttons::playGood(){
	chirpSystem.play({ { 400, 600, 50 } });

}

void RoboCtrl::Buttons::playBad(){
	chirpSystem.play({ { 300, 300, 50 },
					   { 0,   0,   50 },
					   { 300, 300, 50 } });
}

void RoboCtrl::Buttons::playWin(){
	Sound s = { { 600, 400,  200 },
				{ 400, 1000, 200 } };
	chirpSystem.play(s);
}

void RoboCtrl::Buttons::playLose(){
	chirpSystem.play({ { 400, 300, 200 },
					   { 0,   0,   50 },
					   { 300, 200, 200 },
					   { 0,   0,   50 },
					   { 200, 50,  400 } });
}

