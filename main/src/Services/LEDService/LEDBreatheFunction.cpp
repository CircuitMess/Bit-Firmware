#include <algorithm>
#include "LEDBreatheFunction.h"
#include "Util/stdafx.h"
#include <cstdio>

LEDBreatheFunction::LEDBreatheFunction(SingleLED& led, uint32_t count, uint32_t period) : LEDFunction(led), count(count), period(period){
	startTime = millis();
}

LEDBreatheFunction::~LEDBreatheFunction(){
	led.setValue(0);
}

bool LEDBreatheFunction::loop(){
	if(count != 0 && elapsedCount >= count){
		if(led.getValue() != 0){
			led.setValue(0);
		}

		return false;
	}

	const uint64_t elapsedTime = millis() - startTime;
	elapsedCount = elapsedTime / period;

	const uint64_t elapsedInPeriod = elapsedTime % period;
	const float periodPercent = 1.0f * elapsedInPeriod / period;

	const float ledPercent = std::clamp(2.0f * (periodPercent <= 0.5 ? periodPercent : (1.0f - periodPercent)), 0.0f,
										1.0f);

	uint8_t val = ledPercent * 0xFF;

	led.setValue((uint8_t) (ledPercent * 0xFF));

	return true;
}