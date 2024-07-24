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

protected:
	SingleLED& led;
};

#endif //PERSE_ROVER_LEDFUNCTION_H