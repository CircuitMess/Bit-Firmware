#ifndef BIT_LIBRARY_BATTERY_H
#define BIT_LIBRARY_BATTERY_H

#include <hal/gpio_types.h>
#include <atomic>
#include "Util/Threaded.h"
#include "Periph/ADC.h"
#include "Util/Hysteresis.h"
#include "Periph/Timer.h"
#include "Services/ADCReader.h"
#include "Periph/PinOut.h"
#include "Devices/Battery.h"
#include <mutex>
#include <esp_efuse.h>
#include <memory>

class BatteryRev3 : private SleepyThreaded, public Battery {
public:
    BatteryRev3(ADC& adc);

    void begin() override;

    bool isShutdown() const override;

    uint8_t getPerc() const override;
    Level getLevel() const override;

	ChargingState getChargingState() const override;

private:
    static constexpr uint32_t MeasureIntverval = 100;

    static constexpr float VoltEmpty = 3600;
    static constexpr float VoltFull = 4150;
    static constexpr float Factor = 4.0f;
    static constexpr float Offset = 0;
    static constexpr float EmaA = 0.05f;

    static constexpr int CalReads = 10;
    static constexpr float CalExpected = 2500;

    ADC& adc;
    PinOut refSwitch;
    Hysteresis hysteresis;

    std::unique_ptr<ADCReader> readerBatt;
    adc_cali_handle_t caliBatt;

    std::unique_ptr<ADCReader> readerRef;
    adc_cali_handle_t caliRef;

    void calibrate();

    void sample(bool fresh = false);
    bool shutdown = false;

    void sleepyLoop() override;

	//Expected threshold of battery voltage, which will be crossed when charging
	static constexpr uint32_t VoltChargingLow = 4300;
	static constexpr uint32_t VoltChargingHigh = 4400;

	ChargingState chargeState = ChargingState::Unplugged;
	void checkCharging(bool fresh);
	Hysteresis chargeHyst;
	bool wasCharging = false;

};

#endif //BIT_LIBRARY_BATTERY_H
