#include "Robby.h"
#include "math.h"

RoboCtrl::Robby::Robby() : RobotDriver(Robot::Robby), pwm(CTRL_1, LEDC_CHANNEL_2){

}

RoboCtrl::Robby::~Robby(){
	pwm.detach();
}

void RoboCtrl::Robby::hello(){

}

void RoboCtrl::Robby::init(){
	pwm.attach();
	pwm.setDuty(0);
}

void RoboCtrl::Robby::deinit(){
	pwm.detach();
}

void RoboCtrl::Robby::onLoop(uint micros){
	if(period == 0) return;

	t += micros;
	auto duty = 50 + 50 * sin((2 * M_PI / (period * 1000)) * t);
	pwm.setDuty(duty);
	if(t >= period * 1000){
		t = fmod(t, period * 1000);
	}
}

void RoboCtrl::Robby::setPeriod(uint32_t period){
	this->period = period;
}
