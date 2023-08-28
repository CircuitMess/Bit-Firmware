#include "Robots.h"
#include "Util/Events.h"
#include "ChirpSystem.h"
#include "Util/Services.h"
#include "Util/Notes.h"
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

	start();
}

Robot Robots::getInserted(){
	if(!inserted) return Robot::COUNT;
	return (Robot) current;
}

void Robots::sleepyLoop(){
	bool nowInserted = checkInserted();

	if(inserted && !nowInserted){
		inserted = false;
		current = -1;

		Events::post(Facility::Robots, Event { .action = Event::Remove });
	}else if(!inserted && nowInserted){
		uint8_t addr = checkAddr();
		if(addr >= Robot::COUNT){
			// Unknown robot
			addr = Robot::COUNT;
		}else{
			auto audio = (ChirpSystem*) Services.get(Service::Audio);
			if(audio){
				audio->play({
						Chirp{ NOTE_C3, NOTE_C4, 200 },
						Chirp{ 0, 0, 100 },
						Chirp{ NOTE_C4, NOTE_C4, 200 }
				});
			}
		}

		current = addr;
		inserted = true;

		Events::post(Facility::Robots, Event { .action = Event::Insert, .robot = (Robot) current });
	}
}

bool Robots::checkInserted(){
	bool det1 = gpio_get_level((gpio_num_t) DET_1);
	bool det2 = gpio_get_level((gpio_num_t) DET_2);
	return det1 == 0 && det2 == 1;
}

uint8_t Robots::checkAddr(){
	uint8_t addr = 0;
	for(int i = 0; i < 6; i++){
		auto state = gpio_get_level((gpio_num_t) AddrPins[i]);
		if(state){
			addr |= 1 << i;
		}
	}
	return addr;
}
