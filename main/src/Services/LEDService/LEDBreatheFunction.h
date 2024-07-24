#ifndef PERSE_ROVER_LEDBREATHEFUNCTION_H
#define PERSE_ROVER_LEDBREATHEFUNCTION_H

#include "LEDFunction.h"

class LEDBreatheFunction : public LEDFunction {
public:
	LEDBreatheFunction(SingleLED& led, uint32_t count, uint32_t period);

	virtual ~LEDBreatheFunction() override;

protected:
	virtual bool loop() override;

private:
	uint32_t count;
	uint32_t period;
	uint64_t startTime;
	uint32_t elapsedCount = 0;
};

#endif //PERSE_ROVER_LEDBREATHEFUNCTION_H