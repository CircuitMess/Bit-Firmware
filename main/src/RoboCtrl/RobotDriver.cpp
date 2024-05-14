#include "RobotDriver.h"
#include "Util/Services.h"

RoboCtrl::RobotDriver::RobotDriver(Robot type) : type(type){
}

bool RoboCtrl::RobotDriver::checkRobot() const{
	return ((Robots*) Services.get(Service::Robots))->getInserted().robot == type;
}

void RoboCtrl::RobotDriver::loop(uint32_t micros){
	checkCounter += micros;
	if(checkCounter >= CheckInterval){
		checkCounter = 0;
		bool check = checkRobot();

		if(check == this->inserted) return;

		this->inserted = check;
		if(check){
			init();
			inited = true;
		}else{
			deinit();
			inited = false;
		}
	}

	if(inited){
		onLoop(micros);
	}
}

bool RoboCtrl::RobotDriver::isInited() const{
	return inited;
}

void RoboCtrl::RobotDriver::onLoop(uint micros){

}
