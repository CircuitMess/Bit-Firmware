#include "RoboLEDService.h"
#include <esp_log.h>
#include "LEDBlinkFunction.h"
#include "Util/stdafx.h"

static const char* TAG = "LEDService";

RoboLEDService::RoboLEDService() : Threaded("LEDService", 4096, 4, 1), instructionQueue(25){

	start();
}

RoboLEDService::~RoboLEDService(){
	stop(0);
	while(running()){
		delayMillis(10);
	}

	ledFunctions.clear();

	for(auto led: ledDevices){
		delete led.second;
	}
}

void RoboLEDService::remove(LED led){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(ledDevices.contains(led)){
		delete ledDevices[led];
		ledDevices.erase(led);
	}
}

void RoboLEDService::on(LED led, bool interrupt){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = On,
			.interrupt = interrupt
	};

	instructionQueue.post(instruction);
}

void RoboLEDService::off(LED led, bool interrupt){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Off,
			.interrupt = interrupt
	};

	instructionQueue.post(instruction);
}

void RoboLEDService::blink(LED led, uint32_t count /*= 1*/, uint32_t period /*= 1000*/){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Blink,
			.count = count,
			.period = period
	};

	instructionQueue.post(instruction);
}

void RoboLEDService::set(LED led, float percent, bool interrupt){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Set,
			.targetPercent = std::clamp(percent, 0.0f, 100.0f),
			.interrupt = interrupt
	};

	instructionQueue.post(instruction);
}

void RoboLEDService::loop(){
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

void RoboLEDService::onInternal(LED led){
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

void RoboLEDService::offInternal(LED led){
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

void RoboLEDService::blinkInternal(LED led, uint32_t count, uint32_t period){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		ESP_LOGW(TAG, "LED %d is set to blink, but does not exist.", (uint8_t) led);
		return;
	}

	ledFunctions[led] = std::make_unique<LEDBlinkFunction>(*ledDevices[led], count, period);
}

void RoboLEDService::setInternal(LED led, float percent){
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
