#ifndef BIT_FIRMWARE_TWINKLESERVICE_H
#define BIT_FIRMWARE_TWINKLESERVICE_H

#include <unordered_set>
#include "Util/Threaded.h"
#include "Services/LEDService/LEDService.h"

class TwinkleService : public SleepyThreaded {
public:
	TwinkleService();

protected:
	void sleepyLoop() override;
	bool onStart() override;
	void onStop() override;

private:
	LEDService* ledService;
	struct TwinkleInfo {
		uint32_t startTime;
		uint32_t period;
		LED led;
		ledc_channel_t ledcChannel;
	};
	std::vector<TwinkleInfo> registered;
	std::unordered_set<LED> unregistered;

	static constexpr uint32_t MinBreathePeriod = 2000; //ms
	static constexpr uint32_t MaxBreathePeriod = 6000; //ms
	static constexpr uint32_t MaxStartOffset = MinBreathePeriod / 2; //ms

	static constexpr uint8_t PWMLimit = 0x10;

	static constexpr uint8_t MaxRegisteredCount = 5; //5 PWM channels are free to use

	uint32_t getRandomBreathePeriod() const;
};


#endif //BIT_FIRMWARE_TWINKLESERVICE_H
