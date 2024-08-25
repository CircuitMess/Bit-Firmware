#include "Hertz.h"
#include "Util/Services.h"
#include "Devices/SingleDigitalLED.h"

RoboCtrl::Hertz::Hertz() : RobotDriver(Robot::Hertz), pwm(CTRL_1, LEDC_CHANNEL_2), chirpSystem(pwm){
}

RoboCtrl::Hertz::~Hertz(){
	ledService.remove(LED::RobotCtrl1);
	chirpSystem.stop();
}

void RoboCtrl::Hertz::hello(){

}

void RoboCtrl::Hertz::init(){
	ledService.add<SingleDigitalLED>(LED::RobotCtrl2, CTRL_2);
}

void RoboCtrl::Hertz::deinit(){
	ledService.remove(LED::RobotCtrl1);
	chirpSystem.stop();
}

void RoboCtrl::Hertz::playGood(){
	chirpSystem.play({{ 80,   800,  100 },
					  { 0,    0,    50 },
					  { 80,   1000, 150 },
					  { 1000, 80,   150 }});
	ledService.blink(LED::RobotCtrl2, 2, 200);
}

void RoboCtrl::Hertz::playBad(){
	chirpSystem.play({{ 400, 200, 100 },
					  { 0,   0,   100 },
					  { 200, 70,  100 }});
	ledService.blink(LED::RobotCtrl2, 1, 200);
}

void RoboCtrl::Hertz::playDone(){
	Sound s = {{ 400, 300,  100 },
			   { 0,   0,    25 },
			   { 300, 200,  100 },
			   { 0,   0,    25 },
			   { 300, 200,  100 },
			   { 0,   0,    25 },
			   { 800, 1000, 200 }};
	chirpSystem.play(s);
	ledService.blink(LED::RobotCtrl2, 0, 200);
}
