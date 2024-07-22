#include "LEDService.h"
#include <esp_log.h>
#include "Devices/SinglePwmLED.h"
#include "LEDBlinkFunction.h"
#include "LEDBreatheFunction.h"
#include "Pins.hpp"
#include "LEDBreatheToFunction.h"

static const char* TAG = "LEDService";
const std::map<LED, LEDService::PwnMappingInfo> LEDService::PwmMappings = {
		{ LED::Up,    { LED_UP,    0xFF }},
		{ LED::Down,  { LED_DOWN,  0xFF }},
		{ LED::Left,  { LED_LEFT,  0xFF }},
		{ LED::Right, { LED_RIGHT, 0xFF }},
		{ LED::A,     { LED_A,     0xFF }},
		{ LED::B,     { LED_B,     0xFF }},
		{ LED::Menu,  { LED_MENU,  0xFF }}
};

const std::map<LED, LEDService::ExpanderMappingInfo> LEDService::DigitalMappings = {
		{ LED::Up,    { LED_UP,    0xFF }},
		{ LED::Down,  { LED_DOWN,  0xFF }},
		{ LED::Left,  { LED_LEFT,  0xFF }},
		{ LED::Right, { LED_RIGHT, 0xFF }},
		{ LED::A,     { LED_A,     0xFF }},
		{ LED::B,     { LED_B,     0xFF }},
		{ LED::Menu,  { LED_MENU,  0xFF }}
};

LEDService::LEDService() : Threaded("LEDService"), instructionQueue(25){
	for(LED led = (LED) 0; (uint8_t) led < (uint8_t) LED::COUNT; led = (LED) ((uint8_t) led + 1)){
		const bool isExpander = ExpanderMappings.contains(led);
		const bool isPwm = PwmMappings.contains(led);

		if(isExpander && isPwm){
			ESP_LOGE(TAG, "LED %d is marked as both expander and PWM.", (uint8_t) led);
		}else if(isExpander){
			ExpanderMappingInfo ledData = ExpanderMappings.at(led);
			SingleLED* ledDevice = new SingleExpanderLED(aw9523, ledData.pin, ledData.limit);
			ledDevices[led] = ledDevice;
		}else{
			PwnMappingInfo ledData = PwmMappings.at(led);
			SingleLED* ledDevice = new SinglePwmLED(ledData.pin, ledData.channel, ledData.limit);
			ledDevices[led] = ledDevice;
		}
	}

	start();
}

LEDService::~LEDService(){
	ledFunctions.clear();

	for(auto led: ledDevices){
		delete led.second;
	}
}

void LEDService::on(LED led){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = On
	};

	instructionQueue.post(instruction);
}

void LEDService::off(LED led){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Off
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

void LEDService::breathe(LED led, uint32_t period /*= 1000*/){
	LEDInstructionInfo instruction{
			.led = led,
			.instruction = Breathe,
			.count = 0,
			.period = period
	};

	instructionQueue.post(instruction);
}

void LEDService::set(LED led, float percent){
	LEDInstructionInfo instruction{
		.led = led,
		.instruction = Set,
		.targetPercent = std::clamp(percent, 0.0f, 100.0f)
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
			onInternal(instructionInfo.led);
		}else if(instructionInfo.instruction == Off){
			offInternal(instructionInfo.led);
		}else if(instructionInfo.instruction == Blink){
			blinkInternal(instructionInfo.led, instructionInfo.count, instructionInfo.period);
		}else if(instructionInfo.instruction == Breathe){
			breatheInternal(instructionInfo.led, instructionInfo.period);
		}else if(instructionInfo.instruction == BreatheTo){
			breatheToInternal(instructionInfo.led, instructionInfo.targetPercent, instructionInfo.period);
		}else if(instructionInfo.instruction == Set){
			setInternal(instructionInfo.led, instructionInfo.targetPercent);
		}
	}

	for(LED led = (LED) 0; (uint8_t) led < (uint8_t) LED::COUNT; led = (LED) ((uint8_t) led + 1)){
		if(!ledFunctions.contains(led)){
			continue;
		}

		ledFunctions[led]->loop();
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

void LEDService::breatheInternal(LED led, uint32_t period){
	if(ledFunctions.contains(led)){
		ledFunctions.erase(led);
	}

	if(!ledDevices.contains(led)){
		ESP_LOGW(TAG, "LED %d is set to breathe, but does not exist.", (uint8_t) led);
		return;
	}

	ledFunctions[led] = std::make_unique<LEDBreatheFunction>(*ledDevices[led], period);
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
