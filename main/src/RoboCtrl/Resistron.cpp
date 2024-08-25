#include "Resistron.h"
#include "Devices/SingleDigitalLED.h"
#include "Util/Services.h"

RoboCtrl::Resistron::Resistron() : RobotDriver(Robot::Resistron){
}

RoboCtrl::Resistron::~Resistron(){
	ledService.remove(LED::RobotCtrl1);
	ledService.remove(LED::RobotCtrl2);
}

void RoboCtrl::Resistron::blink(){
	repeatNum = 2;
	repeatCounter = 0;
	timer = 0;
	leftRightTime = BlinkTime;
	leftRightAnim = true;
}

void RoboCtrl::Resistron::blinkContinuous(){
	repeatNum = -1;
	repeatCounter = 0;
	timer = 0;
	leftRightTime = BlinkTime;
	leftRightAnim = true;
}

void RoboCtrl::Resistron::blinkTwice(){
	repeatNum = 4;
	repeatCounter = 0;
	timer = 0;
	leftRightTime = BlinkTime;
	leftRightAnim = true;
}

void RoboCtrl::Resistron::hello(){

}

void RoboCtrl::Resistron::init(){
	ledService.add<SingleDigitalLED>(LED::RobotCtrl1, CTRL_1);
	ledService.add<SingleDigitalLED>(LED::RobotCtrl2, CTRL_2);
}

void RoboCtrl::Resistron::deinit(){
	ledService.remove(LED::RobotCtrl1);
	ledService.remove(LED::RobotCtrl2);
}

void RoboCtrl::Resistron::onLoop(uint micros){
	if(!leftRightAnim) return;

	timer += micros;
	if(timer >= leftRightTime * 1000){
		if(ledIndex){
			ledService.set(LED::RobotCtrl1, 100);
			ledService.set(LED::RobotCtrl2, 0);
		}else{
			ledService.set(LED::RobotCtrl1, 0);
			ledService.set(LED::RobotCtrl2, 100);
		}
		ledIndex = !ledIndex;
		timer = 0;
		repeatCounter++;
		if(repeatCounter > repeatNum){
			leftRightAnim = false;
			repeatCounter = 0;
			ledService.off(LED::RobotCtrl1);
			ledService.off(LED::RobotCtrl2);
			return;
		}
	}
}
