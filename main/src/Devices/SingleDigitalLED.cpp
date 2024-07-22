#include "SingleDigitalLED.h"

SingleDigitalLED::SingleDigitalLED(uint8_t pin, uint8_t limit) : SingleLED(limit), pinout(pin){

}


void SingleDigitalLED::write(uint8_t val){
	pinout.write(val > 0);
}
