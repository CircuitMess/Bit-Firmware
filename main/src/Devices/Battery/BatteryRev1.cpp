#include "BatteryRev1.h"
#include "Util/Events.h"
#include <soc/efuse_reg.h>
#include <Util/stdafx.h>
#include <cmath>
#include <driver/gpio.h>
#include "Pins.hpp"

#define MAX_READ 3820 // 4.5V
#define MIN_READ 2870 // 3.6V

BatteryRev1::BatteryRev1(ADC& adc) : SleepyThreaded(MeasureIntverval, "Battery", 3 * 1024, 5, 1), adc(adc),
									 hysteresis({ 0, 4, 15, 30, 50, 70, 90, 100 }, 3){

	adc_unit_t unit;
	adc_channel_t chan;
	ESP_ERROR_CHECK(adc_oneshot_io_to_channel(Pins::get(Pin::BattRead), &unit, &chan));
	assert(unit == adc.getUnit());

	adc.config(chan, {
			.atten = ADC_ATTEN_DB_12,
			.bitwidth = ADC_BITWIDTH_12,
	});

	adcReader = std::make_unique<ADCReader>(adc, chan, nullptr, getVoltOffset(), 1, 0.05, MIN_READ, MAX_READ);


	sample(true);
}

void BatteryRev1::begin(){
	start();
}

uint16_t BatteryRev1::mapRawReading(uint16_t reading){
	return std::round(map((double) reading, MIN_READ, MAX_READ, 3600, 4500));
}

int16_t BatteryRev1::getVoltOffset(){
	int16_t upper = 0, lower = 0;
	esp_efuse_read_field_blob((const esp_efuse_desc_t**) efuse_adc1_low, &lower, 8);
	esp_efuse_read_field_blob((const esp_efuse_desc_t**) efuse_adc1_high, &upper, 8);
	return (upper << 8) | lower;
}

void BatteryRev1::sample(bool fresh){
	if(shutdown) return;

	auto oldLevel = getLevel();

	if(fresh){
		adcReader->resetEma();
		hysteresis.reset(adcReader->getValue());
	}else{
		auto val = adcReader->sample();
		hysteresis.update(val);
	}

	printf("adc: %.2f\n", adcReader->getValue());

	if(oldLevel != getLevel() || fresh){
		Events::post(Facility::Battery, Battery::Event{ .action = Event::LevelChange, .level = getLevel() });
	}

	if(getLevel() == Critical){
		stop(0);
		shutdown = true;
		return;
	}
}

void BatteryRev1::sleepyLoop(){
	if(shutdown) return;
	sample();
}

uint8_t BatteryRev1::getPerc() const{
	return adcReader->getValue();
}

BatteryRev1::Level BatteryRev1::getLevel() const{
	return (Level) hysteresis.get();
}

bool BatteryRev1::isShutdown() const{
	return shutdown;
}

Battery::ChargingState BatteryRev1::getChargingState() const{
	return ChargingState::Unplugged;
}
