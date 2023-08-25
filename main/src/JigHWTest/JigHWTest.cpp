#include "JigHWTest.h"
//#include "SPIFFSChecksum.hpp" TODO - generate SPIFFS checksum file
#include <Pins.hpp>
#include <soc/efuse_reg.h>
#include <esp_efuse.h>
#include <ctime>
#include <iostream>
#include <esp_mac.h>
#include "Util/Services.h"
#include <driver/gptimer.h>
#include <driver/ledc.h>
#include "Devices/Input.h"
#include "Util/Events.h"
#include <esp_adc/adc_oneshot.h>


JigHWTest* JigHWTest::test = nullptr;
Display* JigHWTest::display = nullptr;
LGFX_Device* JigHWTest::canvas = nullptr;


JigHWTest::JigHWTest(){
	display = new Display();
	canvas = &display->getLGFX();

	test = this;

//	tests.push_back({ JigHWTest::SPIFFSTest, "SPIFFS", [](){} });
//	tests.push_back({ JigHWTest::BatteryCalib, "Batt kalib.", [](){} });
//	tests.push_back({ JigHWTest::BatteryCheck, "Batt provjera", [](){} });
}

bool JigHWTest::checkJig(){
	char buf[7];
	int wp = 0;

	uint32_t start = millis();
	int c;
	while(millis() - start < CheckTimeout){
		vTaskDelay(1);
		c = getchar();
		if(c == EOF) continue;
		buf[wp] = (char) c;
		wp = (wp + 1) % 7;

		for(int i = 0; i < 7; i++){
			int match = 0;
			static const char* target = "JIGTEST";

			for(int j = 0; j < 7; j++){
				match += buf[(i + j) % 7] == target[j];
			}

			if(match == 7) return true;
		}
	}

	return false;
}


void JigHWTest::start(){


	uint64_t _chipmacid = 0LL;
	esp_efuse_mac_get_default((uint8_t*) (&_chipmacid));
	printf("\nTEST:begin:%llx\n", _chipmacid);

	canvas->clear(0);
	gpio_set_level((gpio_num_t) PIN_BL, 0);
	rgb();

	canvas->clear(TFT_BLACK);
	canvas->setTextColor(TFT_GOLD);
	canvas->setTextWrap(true, true);
	canvas->setTextDatum(textdatum_t::middle_center);

	canvas->setTextFont(0);
	canvas->setTextSize(1);
	canvas->setCursor(0, 6);

	canvas->print("Clockstar test");
	canvas->setCursor(canvas->width() / 2, 16);
	canvas->println();

	bool pass = true;
	for(const Test& test : tests){
		currentTest = test.name;

		canvas->setTextColor(TFT_WHITE);
		canvas->printf("%s: ", test.name);

		printf("TEST:startTest:%s\n", currentTest);

		bool result = test.test();

		canvas->setTextColor(result ? TFT_GREEN : TFT_RED);
		canvas->printf("%s\n", result ? "PASSED" : "FAILED");

		printf("TEST:endTest:%s\n", result ? "pass" : "fail");

		if(!(pass &= result)){
			if(test.onFail){
				test.onFail();
			}

			break;
		}
	}

	if(!pass){
		printf("TEST:fail:%s\n", currentTest);
		vTaskDelete(nullptr);
	}

	printf("TEST:passall\n");

//------------------------------------------------------


	canvas->print("\n\n");
	canvas->setTextColor(TFT_GREEN);
	canvas->print("All OK!");

	RobotTest();
}

void JigHWTest::rgb(){
	static const char* names[] = { "RED", "GREEN", "BLUE" };
	static const uint16_t colors[] = { TFT_RED, TFT_GREEN, TFT_BLUE };
	for(int i = 0; i < 3; i++){
		canvas->clear(colors[i]);
		canvas->setCursor(20, 40);
		canvas->setTextFont(0);
		canvas->setTextSize(2);
		canvas->print(names[i]);
		vTaskDelay(350);
	}
}

void JigHWTest::log(const char* property, const char* value){
	printf("%s:%s:%s\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, float value){
	printf("%s:%s:%f\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, double value){
	printf("%s:%s:%lf\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, bool value){
	printf("%s:%s:%s\n", currentTest, property, value ? "TRUE" : "FALSE");
}

void JigHWTest::log(const char* property, uint32_t value){
	printf("%s:%s:%lu\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, int32_t value){
	printf("%s:%s:%ld\n", currentTest, property, value);
}

void JigHWTest::log(const char* property, const std::string& value){
	printf("%s:%s:%s\n", currentTest, property, value.c_str());
}

/*

bool JigHWTest::BatteryCalib(){
	if(Battery::getVoltOffset() != 0){
		test->log("calibrated", (int32_t) Battery::getVoltOffset());
		canvas->print("fused. ");
		return true;
	}

	constexpr uint16_t numReadings = 50;
	constexpr uint16_t readDelay = 50;
	uint32_t reading = 0;

	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);

	for(int i = 0; i < numReadings; i++){
		reading += adc1_get_raw(ADC1_CHANNEL_0);
		vTaskDelay(readDelay / portTICK_PERIOD_MS);
	}
	reading /= numReadings;


	uint32_t mapped = Battery::mapRawReading(reading);

	int16_t offset = referenceVoltage - mapped;

	test->log("reading", reading);
	test->log("mapped", mapped);
	test->log("offset", (int32_t) offset);

	if(abs(offset) >= 1000){
		test->log("offset too big, read voltage: ", (uint32_t) mapped);
		return false;
	}

	uint16_t offsetLow = offset & 0b01111111;
	uint16_t offsetHigh = offset >> 7;

	// return true; //TODO - remove early return, burn to efuse

	esp_efuse_write_field_blob((const esp_efuse_desc_t**) efuse_adc1_low, &offsetLow, 7);
	esp_efuse_write_field_blob((const esp_efuse_desc_t**) efuse_adc1_high, &offsetHigh, 9);
	esp_efuse_reset();

	return true;
}


bool JigHWTest::BatteryCheck(){
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_0);

	constexpr uint16_t numReadings = 50;
	constexpr uint16_t readDelay = 10;
	uint32_t reading = 0;

	for(int i = 0; i < numReadings; i++){
		reading += adc1_get_raw(ADC1_CHANNEL_0);
		vTaskDelay(readDelay / portTICK_PERIOD_MS);
	}
	reading /= numReadings;

	uint32_t voltage = Battery::mapRawReading(reading) + Battery::getVoltOffset();
	if(voltage < referenceVoltage - 100 || voltage > referenceVoltage + 100){
		test->log("raw", reading);
		test->log("mapped", (int32_t) Battery::mapRawReading(reading));
		test->log("offset", (int32_t) Battery::getVoltOffset());
		test->log("mapped+offset", voltage);
		return false;
	}

	return true;
}
*/

bool JigHWTest::SPIFFSTest(){

//	auto ret = esp_vfs_spiffs_register(&spiffsConfig);
//	if(ret != ESP_OK){
//		test->log("spiffs", false);
//		return false;
//	}
//
//	for(const auto& f : SPIFFSChecksums){
//		auto file = fopen(f.name, "rb");
//		if(file == nullptr){
//			test->log("missing", f.name);
//			return false;
//		}
//
//		uint32_t sum = calcChecksum(file);
//		fclose(file);
//
//		if(sum != f.sum){
//			test->log("file", f.name);
//			test->log("expected", (uint32_t) f.sum);
//			test->log("got", (uint32_t) sum);
//
//			return false;
//		}
//	}
//
	return true;
}

uint32_t JigHWTest::calcChecksum(FILE* file){
	if(file == nullptr) return 0;

#define READ_SIZE 512

	uint32_t sum = 0;
	uint8_t b[READ_SIZE];
	size_t read = 0;
	while((read = fread(b, 1, READ_SIZE, file))){
		for(int i = 0; i < read; i++){
			sum += b[i];
		}
	}

	return sum;
}

void JigHWTest::RobotTest(){
	/**
	 * Dodatni test checkInserted i adresa 0 (mr. bee)
	 * Kad se izvuče jedan buzz, insert 2 buzza
	 *
	 * Mr. bee ledica radi isto kao i control ledica kao na clockstaru (blink) u zasebnom threadu
	 */

	gpio_config_t io_conf = {
			.pin_bit_mask = (uint64_t) 1 << CTRL_1,
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf);
	gpio_set_level(led_pin, 0);


	gpio_config_t cfg = {};
	for(const auto& pin : AddrPins){
		cfg.pin_bit_mask |= 1ULL << pin;
	}
	for(const auto& pin : { DET_1, DET_2 }){
		cfg.pin_bit_mask |= 1ULL << pin;
	}
	cfg.mode = GPIO_MODE_INPUT;
	gpio_config(&cfg);

	gpio_set_pull_mode((gpio_num_t) DET_1, GPIO_PULLUP_ONLY);
	gpio_set_pull_mode((gpio_num_t) DET_2, GPIO_PULLDOWN_ONLY);


	TaskHandle_t handle;
	xTaskCreate([](void*){
		for(;;){

			gpio_set_level(led_pin, 1);
			vTaskDelay(500);

			gpio_set_level(led_pin, 0);
			vTaskDelay(500);
		}
	}, "BlinkTask", 4096, nullptr, 1, &handle);

	ledc_timer_config_t ledc_timer = {
			.speed_mode       = LEDC_LOW_SPEED_MODE,
			.duty_resolution  = LEDC_TIMER_10_BIT,
			.timer_num        = LEDC_TIMER_0,
			.freq_hz          = 200,
			.clk_cfg          = LEDC_AUTO_CLK,
			.deconfigure      = false
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
			.gpio_num       = PIN_BUZZ,
			.speed_mode     = LEDC_LOW_SPEED_MODE,
			.channel        = LEDC_CHANNEL_0,
			.intr_type      = LEDC_INTR_DISABLE,
			.timer_sel      = LEDC_TIMER_0,

			.duty           = 0,
			.hpoint         = 0,
			.flags = { .output_invert = 1 }
	};
	ledc_channel_config(&ledc_channel);

	bool inserted = false;

	for(;;){
		if(checkInserted() /*&& checkAddr() == TargetRobotAddr */&& !inserted){
			printf("insert\n");
			inserted = true;
			ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (1 << (10 - 1)) - 1);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
			vTaskDelay(BuzzDuration);

			ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
			vTaskDelay(BuzzDuration);

			ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (1 << (10 - 1)) - 1);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
			vTaskDelay(BuzzDuration);

			ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

			//buzz twice
		}

		if((!checkInserted()/* || checkAddr() != TargetRobotAddr*/) && inserted){
			printf("deinsert\n");

			inserted = false;
			ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (1 << (10 - 1)) - 1);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
			vTaskDelay(BuzzDuration);

			ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
			ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
			//buzz once
		}
		vTaskDelay(1);

	}
}

bool JigHWTest::checkInserted(){
	bool det1 = gpio_get_level((gpio_num_t) DET_1);
	bool det2 = gpio_get_level((gpio_num_t) DET_2);
	return det1 == 0 && det2 == 1;
}

uint8_t JigHWTest::checkAddr(){
	uint8_t addr = 0;
	for(int i = 0; i < 6; i++){
		auto state = gpio_get_level((gpio_num_t) AddrPins[i]);
		if(state){
			addr |= 1 << i;
		}
	}
	return addr;
}