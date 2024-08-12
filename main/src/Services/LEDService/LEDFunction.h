#ifndef PERSE_ROVER_LEDFUNCTION_H
#define PERSE_ROVER_LEDFUNCTION_H

#include "Devices/SingleLED.h"

class LEDFunction {
public:
	explicit LEDFunction(SingleLED& led);

	virtual ~LEDFunction() = default;

	/*
	 * @brief Called every loop iteration
	 * @return True if the function is still running, false if it is complete
	 */
	virtual bool loop() = 0;

	void setExitValue(uint8_t exitValue);
	uint8_t getExitValue() const;

protected:
	SingleLED& led;
	uint8_t exitValue;
};

#endif //PERSE_ROVER_LEDFUNCTION_H