#include "BatteryRev3.h"
#include "Pins.hpp"
#include "Util/Events.h"
#include <soc/efuse_reg.h>
#include "Util/stdafx.h"
#include <cmath>
#include <driver/gpio.h>
#include "Util/EfuseMeta.h"

static const char* TAG = "BatteryRev3";

BatteryRev3::BatteryRev3(ADC& adc) : SleepyThreaded(MeasureIntverval, "Battery", 3 * 1024, 5, 1), adc(adc), refSwitch(Pins::get(Pin::CalibVrefEn)),
									 hysteresis({ 0, 4, 15, 30, 50, 70, 90, 100 }, 3), chargeHyst({ 0, VoltChargingLow, VoltChargingHigh }, 20){
	const auto config = [this, &adc](int pin, adc_cali_handle_t& cali, std::unique_ptr<ADCReader>& reader, bool emaAndMap){
		adc_unit_t unit;
		adc_channel_t chan;
		ESP_ERROR_CHECK(adc_oneshot_io_to_channel(pin, &unit, &chan));
		assert(unit == adc.getUnit());

		adc.config(chan, {
				.atten = ADC_ATTEN_DB_2_5,
				.bitwidth = ADC_BITWIDTH_12
		});

		const adc_cali_curve_fitting_config_t curveCfg = {
				.unit_id = unit,
				.chan = chan,
				.atten = ADC_ATTEN_DB_2_5,
				.bitwidth = ADC_BITWIDTH_12
		};

		ESP_ERROR_CHECK(adc_cali_create_scheme_curve_fitting(&curveCfg, &cali));

		if(emaAndMap){
			reader = std::make_unique<ADCReader>(adc, chan, caliBatt, Offset, Factor, EmaA, VoltEmpty, VoltFull);
			reader->setAdjustmentFunction([](float mV){ return -1.1316f * (377.595f - mV); });
		}else{
			reader = std::make_unique<ADCReader>(adc, chan, caliBatt, Offset, Factor);
		}
	};

	config(Pins::get(Pin::BattRead), caliBatt, readerBatt, true);
	config(Pins::get(Pin::BattRead), caliRef, readerRef, false);

	calibrate();

	sample(true);
	sample(true);
	checkCharging(true);
	wasCharging = getChargingState() == Battery::ChargingState::Charging;
}

void BatteryRev3::begin(){
	start();
}

uint8_t BatteryRev3::getPerc() const{
	return readerBatt->getValue();
}

BatteryRev3::Level BatteryRev3::getLevel() const{
	return (Level) hysteresis.get();
}

bool BatteryRev3::isShutdown() const{
	return shutdown;
}

void BatteryRev3::calibrate(){
	refSwitch.on();

	delayMillis(100);
	for(int i = 0; i < CalReads; i++){
		readerRef->sample();
		delayMillis(10);
	}

	float total = 0;
	for(int i = 0; i < CalReads; i++){
		total += readerRef->sample();
		delayMillis(10);
	}

	const float reading = total / (float) CalReads;
	const float offset = CalExpected - reading;
	readerBatt->setMoreOffset(offset);

	refSwitch.off();
	delayMillis(100);

	printf("Calibration: Read %.02f mV, expected %.02f mV. Applying %.02f mV offset.\n", reading, CalExpected, offset);
}

void BatteryRev3::sample(bool fresh){
	if(shutdown) return;

	auto oldLevel = getLevel();

	if(fresh){
		readerBatt->resetEma();
		hysteresis.reset(readerBatt->getValue());
	}else{
		hysteresis.update(readerBatt->sample());
	}

	if(oldLevel != getLevel() || fresh){
		Events::post(Facility::Battery, Battery::Event{ .action = Event::LevelChange, .level = getLevel() });
	}

	if(getLevel() == Critical){
		stop(0);
		shutdown = true;
		return;
	}
}

void BatteryRev3::sleepyLoop(){
	if(shutdown) return;
	checkCharging(false);
	sample();
}

Battery::ChargingState BatteryRev3::getChargingState() const{
	return chargeHyst.get() ? ChargingState::Charging : ChargingState::Unplugged;
}

void BatteryRev3::checkCharging(bool fresh){
	if(shutdown) return;

	const auto val = readerBatt->getUnmappedValue();
	if(fresh){
		chargeHyst.reset(val);
	}else{
		chargeHyst.update(val);
	}

	if(chargeHyst.get() == 1){
		if(!wasCharging){
			wasCharging = true;
			ESP_LOGI(TAG, "Charger plugged in!");
			Events::post(Facility::Battery, Battery::Event{ .action = Event::Charging, .chargingState = ChargingState::Charging });
		}
	}else if(wasCharging){
		wasCharging = false;
		sample(true);
		ESP_LOGI(TAG, "Charger unplugged!");
		Events::post(Facility::Battery, Battery::Event{ .action = Event::Charging, .chargingState = ChargingState::Unplugged });
	}
}
