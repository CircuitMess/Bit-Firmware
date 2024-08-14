#include "LEDService.h"
#include <esp_log.h>
#include "Devices/SinglePwmLED.h"
#include "LEDBlinkFunction.h"
#include "LEDBreatheFunction.h"
#include "Pins.hpp"
#include "LEDBreatheToFunction.h"

static const char* TAG = "LEDService";

LEDService::LEDService() : Threaded("LEDService", 4096, 4, 1), instructionQueue(25){

	start();
}

LEDService::~LEDService(){
	ledFunctions.clear();

	for(auto led: ledDevices){
		delete led.second;
	}
}

void LEDService::remove(LED led){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(ledDevices.contains(led)){
		delete ledDevices[led];
		ledDevices.erase(led);
	}
}

void LEDService::on(LED led, bool interrupt){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = On,
			.interrupt = interrupt
	};

	instructionQueue.post(instruction);
}

void LEDService::off(LED led, bool interrupt){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Off,
			.interrupt = interrupt
	};

	instructionQueue.post(instruction);
}

void LEDService::blink(LED led, uint32_t count /*= 1*/, uint32_t period /*= 1000*/){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Blink,
			.count = count,
			.period = period
	};

	instructionQueue.post(instruction);
}

void LEDService::breathe(LED led, uint32_t count /*= 0*/, uint32_t period /*= 1000*/){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Breathe,
			.count = count,
			.period = period
	};

	instructionQueue.post(instruction);
}

void LEDService::set(LED led, float percent, bool interrupt){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Set,
			.targetPercent = std::clamp(percent, 0.0f, 100.0f),
			.interrupt = interrupt
	};

	instructionQueue.post(instruction);
}

void LEDService::breatheTo(LED led, float targetPercent, uint32_t duration){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = BreatheTo,
			.period = duration,
			.targetPercent = std::clamp(targetPercent, 0.0f, 100.0f)
	};

	instructionQueue.post(instruction);
}

void LEDService::loop(){
	for(LEDInstructionInfo instructionInfo; instructionQueue.get(instructionInfo, 10);){
		if(instructionInfo.instruction == On){
			if(ledFunctions.contains(instructionInfo.led) && !instructionInfo.interrupt){
				ledFunctions[instructionInfo.led]->setExitValue(0xFF);
			}else{
				onInternal(instructionInfo.led);
			}
		}else if(instructionInfo.instruction == Off){
			if(ledFunctions.contains(instructionInfo.led) && !instructionInfo.interrupt){
				ledFunctions[instructionInfo.led]->setExitValue(0);
			}else{
				offInternal(instructionInfo.led);
			}
		}else if(instructionInfo.instruction == Blink){
			blinkInternal(instructionInfo.led, instructionInfo.count, instructionInfo.period);
		}else if(instructionInfo.instruction == Breathe){
			breatheInternal(instructionInfo.led, instructionInfo.count, instructionInfo.period);
		}else if(instructionInfo.instruction == BreatheTo){
			breatheToInternal(instructionInfo.led, instructionInfo.targetPercent, instructionInfo.period);
		}else if(instructionInfo.instruction == Set){
			if(ledFunctions.contains(instructionInfo.led) && !instructionInfo.interrupt){
				ledFunctions[instructionInfo.led]->setExitValue(instructionInfo.targetPercent * 0xFF);
			}else{
				setInternal(instructionInfo.led, instructionInfo.targetPercent);
			}
		}
	}

	for(LED led = (LED) 0; (uint8_t) led < (uint8_t) LED::COUNT; led = (LED) ((uint8_t) led + 1)){
		if(!ledFunctions.contains(led)){
			continue;
		}

		bool ret = ledFunctions[led]->loop();
		if(!ret){
			setInternal(led, (float) ledFunctions[led]->getExitValue() / 255.0);
			ledFunctions.erase(led);
		}
	}
}

void LEDService::onInternal(LED led){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		return;
	}

	if(ledDevices[led] == nullptr){
		return;
	}

	ledDevices[led]->setValue(0xFF);
}

void LEDService::offInternal(LED led){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		return;
	}

	if(ledDevices[led] == nullptr){
		return;
	}

	ledDevices[led]->setValue(0);
}

void LEDService::blinkInternal(LED led, uint32_t count, uint32_t period){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		ESP_LOGW(TAG, "LED %d is set to blink, but does not exist.", (uint8_t) led);
		return;
	}

	ledFunctions[led] = std::make_unique<LEDBlinkFunction>(*ledDevices[led], count, period);
}

void LEDService::breatheInternal(LED led, uint32_t count, uint32_t period){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		ESP_LOGW(TAG, "LED %d is set to breathe, but does not exist.", (uint8_t) led);
		return;
	}

	ledFunctions[led] = std::make_unique<LEDBreatheFunction>(*ledDevices[led], count, period);
}

void LEDService::setInternal(LED led, float percent){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		return;
	}

	if(ledDevices[led] == nullptr){
		return;
	}

	ledDevices[led]->setValue(0xFF * percent);
}

void LEDService::breatheToInternal(LED led, float targetPercent, uint32_t duration){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		ESP_LOGW(TAG, "LED %d is set to breathe to value, but does not exist.", (uint8_t) led);
		return;
	}

	ledFunctions[led] = std::make_unique<LEDBreatheToFunction>(*ledDevices[led], targetPercent, duration);
}
