#include <esp_log.h>
#include "TwinkleService.h"
#include "Util/Services.h"
#include "esp_random.h"
#include "Util/stdafx.h"
#include "Devices/SingleDigitalLED.h"
#include "Devices/SinglePwmLED.h"
#include "Pins.hpp"

static const char* TAG = "TwinkleService";

const std::map<LED, TwinkleService::PwnMappingInfo> TwinkleService::PwmMappings = {
		{ LED::Up,    { (gpio_num_t) LED_UP,    PWMLimit }},
		{ LED::Down,  { (gpio_num_t) LED_DOWN,  PWMLimit }},
		{ LED::Left,  { (gpio_num_t) LED_LEFT,  PWMLimit }},
		{ LED::Right, { (gpio_num_t) LED_RIGHT, PWMLimit }},
		{ LED::A,     { (gpio_num_t) LED_A,     PWMLimit }},
		{ LED::B,     { (gpio_num_t) LED_B,     PWMLimit }},
		{ LED::Menu,  { (gpio_num_t) LED_MENU,  PWMLimit }}
};

TwinkleService::TwinkleService() : SleepyThreaded(500, "TwinkleService", 3000, 0, 0),
								   ledService((LEDService*) Services.get(Service::LED)){
}

void TwinkleService::sleepyLoop(){
	const uint32_t currentTime = millis();

	for(auto it = registered.begin(); it != registered.end();){
		TwinkleInfo& info = *it;


		if(info.startTime > currentTime){
			++it;
			continue;
		}

		if(info.startTime + info.period < currentTime){
			ESP_LOGD(TAG, "LED %d done", (uint8_t) info.led);

			ledService->remove(info.led);

			uint32_t currentTime = millis();
			uint32_t offset = esp_random() % MaxStartOffset;
			auto it2 = unregistered.begin();
			std::advance(it2, esp_random() % unregistered.size());
			LED led = *it2;

			TwinkleInfo newInfo = { currentTime + offset, getRandomBreathePeriod(), led, info.ledcChannel };

			unregistered.insert(info.led);
			unregistered.erase(led);


			auto& pwmInfo = PwmMappings.at(led);
			ledService->add<SinglePwmLED>(led, pwmInfo.pin, newInfo.ledcChannel, pwmInfo.limit);
			ledService->breathe(led, 1, newInfo.period);

			ESP_LOGD(TAG, "ledService add %d on channel %d", (uint8_t) led, newInfo.ledcChannel);

			info = newInfo; //swap old TwinkleInfo with new
			++it;
			continue;
		}

		++it;

		ESP_LOGD(TAG, "LED %d still running", (uint8_t) info.led);
	}
}

bool TwinkleService::onStart(){
	registered.clear();
	unregistered.clear();
	unregistered.insert({ LED::Up, LED::Down, LED::Left, LED::Right, LED::A, LED::B, LED::Menu });

	for(int i = 0; i < MaxRegisteredCount; ++i){
		//Same as registerRandomLED, but without actual LEDs starting
		//Used only for randomizing the starting offsets
		auto ledcChannel = ((ledc_channel_t) (i + 3));

		uint32_t currentTime = millis();
		uint32_t offset = esp_random() % MaxStartOffset;
		auto it = unregistered.begin();
		std::advance(it, esp_random() % unregistered.size());
		LED led = *it;

		TwinkleInfo info = { currentTime + offset, 0, led, ledcChannel };
		registered.push_back(info);
		unregistered.erase(led);
	}

	return true;
}

void TwinkleService::onStop(){
	for(auto& i: PwmMappings){
		ledService->remove(i.first);
		ledService->add<SingleDigitalLED>(i.first, i.second.pin);
	}
}

uint32_t TwinkleService::getRandomBreathePeriod() const{
	return MinBreathePeriod + (esp_random() % (MaxBreathePeriod - MinBreathePeriod));
}

/*
void TwinkleService::registerRandomLED(ledc_channel_t ledcChannel){
	uint32_t currentTime = millis();
	uint32_t offset = esp_random() % MaxStartOffset;
	auto it = unregistered.begin();
	std::advance(it, esp_random() % unregistered.size());
	LED led = *it;

	TwinkleInfo info = { currentTime + offset, getRandomBreathePeriod(), led, ledcChannel };
	registered.push_back(info);
	unregistered.erase(led);

	auto& pwmInfo = PwmMappings.at(led);
	ledService->add<SinglePwmLED>(led, pwmInfo.pin, ledcChannel, pwmInfo.limit);
	ledService->breathe(led, 1, info.period);

	ESP_LOGD(TAG, "ledService add %d on channel %d", (uint8_t) led, ledcChannel);
}
*/
