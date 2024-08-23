#include "Robots.h"
#include "Util/Events.h"
#include <driver/gpio.h>

Robots::Robots() : SleepyThreaded(CheckInterval, "Robots", 2 * 1024, 5, 1){
	gpio_config_t cfg = {};
	for(const auto& pin : AddrPins){
		cfg.pin_bit_mask |= 1ULL << pin;
	}
	for(const auto& pin : CtrlPins){
		cfg.pin_bit_mask |= 1ULL << pin;
	}
	for(const auto& pin : { DET_1, DET_2 }){
		cfg.pin_bit_mask |= 1ULL << pin;
	}
	cfg.mode = GPIO_MODE_INPUT;
	gpio_config(&cfg);

	gpio_set_pull_mode((gpio_num_t) DET_1, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode((gpio_num_t) DET_2, GPIO_PULLDOWN_ONLY);

	for(const auto& pin : AddrPins){
		gpio_set_pull_mode((gpio_num_t) pin, GPIO_PULLUP_ONLY);
	}

	Robots::sleepyLoop();
	start();
}

Robots::~Robots(){
	stop();
}

RobotData Robots::getInserted(){
	if(!inserted) return { Robot::COUNT, Token::COUNT};
	return current;
}

void Robots::sleepyLoop(){
	const bool nowInserted = checkInserted();

	if(inserted && !nowInserted){
		inserted = false;
		current = { Robot::COUNT, Token::COUNT };

		Events::post(Facility::Robots, Event { .action = Event::Remove });
	}else if(!inserted && nowInserted){
		uint8_t addr = checkAddr();
		if(addr == 62){
			current.token = (Token) checkToken();
		}else{
			current.token = Token::COUNT;
		}

		if(addr >= (uint8_t) Robot::COUNT){
			// Unknown robot
			addr = (uint8_t) Robot::COUNT;
		}

		current.robot = (Robot) addr;

		inserted = true;

		Events::post(Facility::Robots, Event { .action = Event::Insert, .robot = current });
	}
}

bool Robots::checkInserted(){
	bool det1 = gpio_get_level((gpio_num_t) DET_1);
	bool det2 = gpio_get_level((gpio_num_t) DET_2);
	return det1 == 0 && det2 == 1;
}

uint8_t Robots::checkAddr() const{
	uint8_t addr = 0;

	// Check regular robots
	for(int i = 0; i < 6; i++){
		auto state = gpio_get_level((gpio_num_t) AddrPins[i]);
		if(state){
			addr |= 1 << i;
		}
	}

	return addr;
}

uint8_t Robots::checkToken() const{
	uint8_t addr = 0;

	for(int i = 0; i < 6; i++){
		auto state = gpio_get_level((gpio_num_t) CtrlPins[i]);
		if(state){
			addr |= 1 << i;
		}
	}

	return addr;
}

bool Robots::testerBob(){
	return checkInserted() && checkAddr() == TesterBobAddr;
}
