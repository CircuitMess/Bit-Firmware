#include "JigHWTest.h"
#include "SPIFFSChecksum.hpp"
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
#include "Services/ChirpSystem.h"


JigHWTest* JigHWTest::test = nullptr;
Display* JigHWTest::display = nullptr;
LGFX_Device* JigHWTest::canvas = nullptr;


JigHWTest::JigHWTest(){
	display = new Display();
	canvas = &display->getLGFX();

	test = this;

	// tests.push_back({ JigHWTest::Robot, "Konektor", [](){} });
	tests.push_back({ JigHWTest::Buttons, "Gumbi", [](){} });
	tests.push_back({ JigHWTest::SPIFFSTest, "SPIFFS", [](){} });
	tests.push_back({ JigHWTest::BatteryCalib, "Batt kalib", [](){} });
	tests.push_back({ JigHWTest::BatteryCheck, "Batt provjera", [](){} });
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

	gpio_config_t io_conf = {
			.pin_bit_mask = (1ULL << CTRL_1) | (1ULL << PIN_BL),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf);
	gpio_set_level(led_pin, 0);

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

	canvas->print("Bit test\n\n");

	bool pass = true;
	for(const Test& test : tests){
		currentTest = test.name;

		canvas->setTextColor(TFT_WHITE);
		canvas->printf("%s: ", test.name);

		printf("TEST:startTest:%s\n", currentTest);

		bool result = test.test();

		canvas->setTextColor(result ? TFT_GREEN : TFT_RED);
		canvas->printf("%s\n", result ? "OK" : "FAIL");

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
		return;
	}

	printf("TEST:passall\n");

//------------------------------------------------------

	canvas->setTextColor(TFT_GREEN);
	canvas->print("\nTEST GOTOV. SVE OK.");

	for(;;){
		gpio_set_level((gpio_num_t) LED_MENU, 1);
		vTaskDelay(500);

		gpio_set_level((gpio_num_t) LED_MENU, 0);
		vTaskDelay(500);
	}
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

void JigHWTest::instr(const char* msg){
	canvas->setTextColor(TFT_GOLD);
	canvas->print(msg);
	canvas->print(" ");
}

bool JigHWTest::BatteryCalib(){
	/*if(Battery::getVoltOffset() != 0){
		test->log("calibrated", (int32_t) Battery::getVoltOffset());
		test->instr("fused.");
		return true;
	}

	ADC adc((gpio_num_t) PIN_BATT);

	constexpr uint16_t numReadings = 200;
	constexpr uint16_t readDelay = 10;
	uint32_t reading = 0;

	for(int i = 0; i < numReadings; i++){
		reading += adc.sample();
		vTaskDelay(readDelay);
	}
	reading /= numReadings;


	uint32_t mapped = Battery::mapRawReading(reading);

	int16_t offset = referenceVoltage - mapped;

	test->log("reading", reading);
	test->log("mapped", mapped);
	test->log("offset", (int32_t) offset);

	if(abs(offset) >= 500){
		test->log("offset too big, read voltage: ", (uint32_t) mapped);
		return false;
	}

	uint8_t offsetLow = offset & 0xff;
	uint8_t offsetHigh = (offset >> 8) & 0xff;

	// return true; //TODO - remove early return, burn to efuse

	esp_efuse_batch_write_begin();
	esp_efuse_write_field_blob((const esp_efuse_desc_t**) efuse_adc1_low, &offsetLow, 8);
	esp_efuse_write_field_blob((const esp_efuse_desc_t**) efuse_adc1_high, &offsetHigh, 8);
	esp_efuse_batch_write_commit();
	*/

	return true;
}


bool JigHWTest::BatteryCheck(){
	/*ADC adc((gpio_num_t) PIN_BATT);

	constexpr uint16_t numReadings = 200;
	constexpr uint16_t readDelay = 10;
	uint32_t reading = 0;

	for(int i = 0; i < numReadings; i++){
		reading += (int) adc.sample();
		vTaskDelay(readDelay);
	}
	reading /= numReadings;

	uint32_t voltage = Battery::mapRawReading(reading) + Battery::getVoltOffset();

	if(voltage < referenceVoltage - 200 || voltage > referenceVoltage + 200){
		test->log("raw", reading);
		test->log("mapped", (int32_t) Battery::mapRawReading(reading));
		test->log("offset", (int32_t) Battery::getVoltOffset());
		test->log("mapped+offset", voltage);
		return false;
	}
	*/

	return true;
}


bool JigHWTest::SPIFFSTest(){
	auto ret = esp_vfs_spiffs_register(&spiffsConfig);
	if(ret != ESP_OK){
		test->log("spiffs", false);
		return false;
	}

	for(const auto& f : SPIFFSChecksums){
		auto file = fopen(f.name, "rb");
		if(file == nullptr){
			test->log("missing", f.name);
			return false;
		}

		uint32_t sum = calcChecksum(file);
		fclose(file);

		if(sum != f.sum){
			test->log("file", f.name);
			test->log("expected", (uint32_t) f.sum);
			test->log("got", (uint32_t) sum);

			return false;
		}
	}

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

bool JigHWTest::Buttons(){
	PWM buzzPwm(PIN_BUZZ, LEDC_CHANNEL_0);
	ChirpSystem audio(buzzPwm);
	const auto buzz = [&audio](){
		audio.play({ Chirp { 200, 200, 100 } });
	};

	EventQueue evts(12);
	Input input(true);
	vTaskDelay(200);
	Events::listen(Facility::Input, &evts);

	std::unordered_set<Input::Button> pressed;
	std::unordered_set<Input::Button> released;

	test->instr("Pritisni sve\ngumbe redom.");
	audio.play({
		Chirp { 200, 200, 100 },
		Chirp { 0, 0, 50 },
		Chirp { 200, 200, 100 },
		Chirp { 0, 0, 50 },
		Chirp { 200, 200, 100 }
	});

	gpio_config_t cfg = {
			.pin_bit_mask = ((uint64_t) 1) << LED_UP | ((uint64_t) 1) << LED_DOWN | ((uint64_t) 1) << LED_LEFT | ((uint64_t) 1) << LED_RIGHT |
							((uint64_t) 1) << LED_A | ((uint64_t) 1) << LED_B | ((uint64_t) 1) << LED_MENU,
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&cfg);

	for(auto btn : { LED_A, LED_B, LED_UP, LED_DOWN, LED_RIGHT, LED_LEFT, LED_MENU }){
		gpio_set_level((gpio_num_t) btn, 1);
	}


	for(;;){
		Event evt{};
		if(!evts.get(evt, portMAX_DELAY)) continue;

		auto data = (Input::Data*) evt.data;
		if(data->action == Input::Data::Press){
			pressed.insert(data->btn);
			switch(data->btn){
				case Input::Up:
					gpio_set_level((gpio_num_t)LED_UP, 0);
					break;
				case Input::Down:
					gpio_set_level((gpio_num_t)LED_DOWN, 0);
					break;
				case Input::Left:
					gpio_set_level((gpio_num_t)LED_LEFT, 0);
					break;
				case Input::Right:
					gpio_set_level((gpio_num_t)LED_RIGHT, 0);
					break;
				case Input::A:
					gpio_set_level((gpio_num_t)LED_A, 0);
					break;
				case Input::B:
					gpio_set_level((gpio_num_t)LED_B, 0);
					break;
				case Input::Menu:
					gpio_set_level((gpio_num_t)LED_MENU, 0);
					break;
			}
			buzz();
		}else{
			released.insert(data->btn);
		}

		free(evt.data);
		if(pressed.size() == 7 && released.size() == 7) break;
	}

	Events::unlisten(&evts);

	vTaskDelay(300);
	audio.play({
		   Chirp { 200, 200, 100 },
		   Chirp { 0, 0, 50 },
		   Chirp { 200, 200, 100 }
   });
	vTaskDelay(500);

	gpio_set_level((gpio_num_t)LED_UP, 0);
	gpio_set_level((gpio_num_t)LED_DOWN, 0);
	gpio_set_level((gpio_num_t)LED_LEFT, 0);
	gpio_set_level((gpio_num_t)LED_RIGHT, 0);
	gpio_set_level((gpio_num_t)LED_A, 0);
	gpio_set_level((gpio_num_t)LED_B, 0);
	gpio_set_level((gpio_num_t)LED_MENU, 0);

	return true;
}

bool JigHWTest::Robot(){
	Robots rob;
	gpio_set_direction((gpio_num_t) CTRL_1, GPIO_MODE_OUTPUT);
	gpio_set_level((gpio_num_t) CTRL_1, 0);

	if(rob.getInserted().robot == Robot::Bob){
		test->instr("Krivi Bob.\nUzmi iz kutije za\ntestiranje.");
		test->log("bob", "krivi");
		return false;
	}else if(!rob.testerBob()){
		test->instr("Bob nije\numetnut.");
		test->log("bob", false);
		return false;
	}

	ThreadedClosure blinker([](){
		for(int i = 0; i < 2; i++){
			gpio_set_level((gpio_num_t) CTRL_1, 1);
			delayMillis(100);
			gpio_set_level((gpio_num_t) CTRL_1, 0);
			delayMillis(100);
		}
		delayMillis(1000);
	}, "Blinker", 2048);

	PWM buzzPwm(PIN_BUZZ, LEDC_CHANNEL_0);
	ChirpSystem audio(buzzPwm);
	const auto buzz = [&audio](){
		audio.play({ Chirp { 200, 200, 100 } });
	};
	const auto buzzDbl = [&audio](){
		audio.play({
			Chirp { 200, 200, 100 },
			Chirp { 0, 0, 100 },
			Chirp { 200, 200, 100 }
		});
	};

	EventQueue evts(12);
	Events::listen(Facility::Robots, &evts);

	const auto out = [&evts, &blinker](){
		Events::unlisten(&evts);
		blinker.stop();
		gpio_set_level((gpio_num_t) CTRL_1, 0);
		vTaskDelay(500);
	};

	auto waitEvt = [&evts](){
		Robots::Event rEvt;
		for(;;){
			Event evt{};
			if(!evts.get(evt, portMAX_DELAY)) continue;
			rEvt = *((Robots::Event*) evt.data);
			free(evt.data);
			break;
		}
		return rEvt;
	};

	test->instr("Makni Boba.\n");
	blinker.start();

	auto evt = waitEvt();
	if(evt.action != Robots::Event::Remove){
		test->log("rem", false);
		test->instr("Nezz.");
		out();
		return false;
	}
	buzz();

	test->instr("Sad ga vrati.");
	evt = waitEvt();

	if(evt.action != Robots::Event::Insert){
		test->log("ins", false);
		test->instr("Nezz.");
		out();
		return false;
	}

	if(evt.robot.robot == Robot::COUNT){
		if(rob.testerBob()){
			buzzDbl();
			vTaskDelay(500);

			out();
			return true;
		}else{
			test->instr("Los konektor");
			test->log("err", "konektor");
			out();
			return false;
		}
	}else if(evt.robot.robot == Robot::Bob){
		test->instr("Krivi\nBob.  Uzmi iz kutije\nza testiranje.");
		test->log("err", "bob");
		out();
		return false;
	}else{
		test->instr("Krivi robot.");
		test->log("err", "robot");
		Events::unlisten(&evts);
		out();
		return false;
	}

	out();
	return false;
}
