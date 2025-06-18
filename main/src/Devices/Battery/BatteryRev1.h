#ifndef BIT_FIRMWARE_BATTERYREV1_H
#define BIT_FIRMWARE_BATTERYREV1_H

#include <esp_efuse.h>
#include <memory>
#include "Util/Threaded.h"
#include "Devices/Battery.h"
#include "Util/Hysteresis.h"
#include "Services/ADCReader.h"
#include "Periph/ADC.h"

class BatteryRev1 : private SleepyThreaded, public Battery {
public:
	BatteryRev1(ADC& adc);
	void begin() override;


	uint8_t getPerc() const override;
	Level getLevel() const override;

	static int16_t getVoltOffset();
	static uint16_t mapRawReading(uint16_t reading);

	bool isShutdown() const override;

	ChargingState getChargingState() const override;
private:
	static constexpr uint32_t MeasureIntverval = 100;

	ADC& adc;
	std::unique_ptr<ADCReader> adcReader;
	Hysteresis hysteresis;


	void sleepyLoop() override;

	void sample(bool fresh = false);
	bool shutdown = false;

	static constexpr esp_efuse_desc_t adc1_low = { EFUSE_BLK3, 0, 8 };
	static constexpr const esp_efuse_desc_t* efuse_adc1_low[] = { &adc1_low, nullptr };
	static constexpr esp_efuse_desc_t adc1_high = { EFUSE_BLK3, 8, 8 };
	static constexpr const esp_efuse_desc_t* efuse_adc1_high[] = { &adc1_high, nullptr };
};


#endif //BIT_FIRMWARE_BATTERYREV1_H
