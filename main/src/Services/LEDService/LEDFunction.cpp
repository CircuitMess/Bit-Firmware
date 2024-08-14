#include "LEDFunction.h"

LEDFunction::LEDFunction(SingleLED& led) : led(led), exitValue(led.getValue()){}

void LEDFunction::setExitValue(uint8_t exitValue){
	LEDFunction::exitValue = exitValue;
}

uint8_t LEDFunction::getExitValue() const{
	return exitValue;
}
