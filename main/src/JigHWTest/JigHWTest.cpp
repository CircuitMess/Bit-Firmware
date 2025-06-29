#include "JigHWTest.h"
#include "SPIFFSChecksum.hpp"
#include <Pins.hpp>
#include <soc/efuse_reg.h>
#include <esp_efuse.h>
#include <iostream>
#include <esp_mac.h>
#include "Util/Services.h"
#include <driver/ledc.h>
#include "Devices/Input.h"
#include "Util/Events.h"
#include <esp_adc/adc_oneshot.h>
#include "Services/ChirpSystem.h"
#include "Util/EfuseMeta.h"
#include "Util/Notes.h"

JigHWTest* JigHWTest::test = nullptr;
Display* JigHWTest::display = nullptr;
LGFX_Device* JigHWTest::panel = nullptr;
LGFX_Sprite* JigHWTest::canvas = nullptr;
Input* JigHWTest::input = nullptr;


JigHWTest::JigHWTest(){
	display = new Display(EfuseMeta::getHardcodedRevision());
	panel = &display->getLGFX();
	canvas = new LGFX_Sprite(panel);
	canvas->setColorDepth(lgfx::rgb565_2Byte);
	canvas->createSprite(128, 128);
	input = new Input(true);

	test = this;

	tests.push_back({ JigHWTest::VoltReferenceCheck, "Voltage ref", [](){ gpio_set_level((gpio_num_t) Pins::get(Pin::CalibVrefEn), 0); }});
	tests.push_back({ JigHWTest::SPIFFSTest, "SPIFFS", [](){}});
	tests.push_back({ JigHWTest::BatteryCheck, "Battery check", [](){}});
	tests.push_back({ JigHWTest::Buttons, "Buttons", [](){}});
	tests.push_back({ JigHWTest::HWVersion, "HW rev", [](){ esp_efuse_batch_write_cancel(); }});
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

	gpio_config_t cfg = {
			.pin_bit_mask = ((uint64_t) 1) << Pins::get(Pin::LedBl),
			.mode = GPIO_MODE_OUTPUT,
			.pull_up_en = GPIO_PULLUP_DISABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&cfg);
	gpio_set_level((gpio_num_t) Pins::get(Pin::LedBl), 0);

	canvas->clear(TFT_BLACK);
	canvas->setTextColor(TFT_GOLD);
	canvas->setTextWrap(false, false);
	canvas->setTextDatum(textdatum_t::middle_center);
	canvas->pushSprite(0, 0);

	canvas->setTextFont(0);
	canvas->setTextSize(1);

	canvas->drawString("Bit Hardware Test", canvas->width() / 2, 6);
	canvas->println();

	canvas->pushSprite(0, 0);

	canvas->setCursor(0, 16);

	bool pass = true;
	for(const Test& test : tests){
		currentTest = test.name;

		canvas->setTextColor(TFT_WHITE);
		canvas->printf("%s: ", test.name);
		canvas->pushSprite(0, 0);


		printf("TEST:startTest:%s\n", currentTest);

		bool result = test.test();

		canvas->setTextColor(result ? TFT_SILVER : TFT_ORANGE);
		canvas->printf("%s\n", result ? "PASS" : "FAIL");
		canvas->pushSprite(0, 0);

		printf("TEST:endTest:%s\n", result ? "pass" : "fail");

		if(!(pass &= result)){
			if(test.onFail){
				test.onFail();
			}

			break;
		}
	}

	if(pass){
		printf("TEST:passall\n");
	}else{
		printf("TEST:fail:%s\n", currentTest);
	}


	//------------------------------------------------------
	canvas->print("\n");
	canvas->setTextColor(pass ? TFT_BLUE : TFT_ORANGE);
	canvas->drawCentreString(pass ? "All OK!" : "FAIL!", canvas->width() / 2, canvas->getCursorY());
	canvas->pushSprite(0, 0);

	bool painted = false;
	const auto color = pass ? TFT_GREEN : TFT_RED;
	auto flashTime = 0;
	bool tone = false;
	const uint16_t note = NOTE_C6 + ((rand() * 20) % 400) - 200; //NOTE_C6 = 1047

	auto buzzPwm = new PWM(Pins::get(Pin::Buzz), LEDC_CHANNEL_0);
	auto audio = new ChirpSystem(*buzzPwm);

	for(;;){
		if(millis() - flashTime >= 500){
			for(int x = 0; x < canvas->width(); x++){
				for(int y = 0; y < canvas->height(); y++){
					const auto previousPixel = canvas->readPixel(x, y);
					if(!painted && previousPixel == TFT_BLACK){
						canvas->drawPixel(x, y, color);
					}else if(painted && previousPixel == color){
						canvas->drawPixel(x, y, TFT_BLACK);
					}
				}
			}

			flashTime = millis();
			painted = !painted;
			canvas->pushSprite(0, 0);
		}

		auto press = false;
		for(int i = 0; i < ButtonCount; i++){
			if(input->isPressed((Input::Button) i)){
				press = true;
				break;
			}
		}

		if(press && !tone){
			audio->play({{ note, note, 2000 }});
			tone = true;
		}else if(!press && tone){
			audio->stop();
			tone = false;
		}

		delayMillis(10);
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


bool JigHWTest::BatteryCheck(){
	ADC adc(ADC_UNIT_1);

	const auto config = [&adc](int pin, adc_cali_handle_t& cali, std::unique_ptr<ADCReader>& reader){
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

		static constexpr float Factor = 4.0f;
		static constexpr float Offset = 0;

		reader = std::make_unique<ADCReader>(adc, chan, cali, Offset, Factor);
	};

	adc_cali_handle_t cali;
	std::unique_ptr<ADCReader> reader;
	config(Pins::get(Pin::BattRead), cali, reader);

	constexpr uint16_t numReadings = 50;
	constexpr uint16_t readDelay = 10;
	uint32_t reading = 0;

	for(int i = 0; i < numReadings; i++){
		reading += reader->sample();
		vTaskDelay(readDelay / portTICK_PERIOD_MS);
	}
	reading /= numReadings;

	test->log("reading", reading);

	if(reading < BatVoltageMinimum){
		return false;
	}

	return true;
}

bool JigHWTest::VoltReferenceCheck(){
	const gpio_num_t RefSwitch = (gpio_num_t) Pins::get(Pin::CalibVrefEn);
	gpio_set_direction(RefSwitch, GPIO_MODE_OUTPUT);
	gpio_set_level(RefSwitch, 1);

	delayMillis(100);

	ADC adc(ADC_UNIT_1);

	const auto config = [&adc](int pin, adc_cali_handle_t& cali, std::unique_ptr<ADCReader>& reader){
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

		static constexpr float Factor = 4.0f;
		static constexpr float Offset = 0;

		reader = std::make_unique<ADCReader>(adc, chan, cali, Offset, Factor);
	};

	adc_cali_handle_t cali;
	std::unique_ptr<ADCReader> reader;
	config(Pins::get(Pin::BattRead), cali, reader);

	constexpr uint16_t numReadings = 50;
	constexpr uint16_t readDelay = 10;
	uint32_t reading = 0;

	for(int i = 0; i < numReadings; i++){
		reading += reader->sample();
		vTaskDelay(readDelay / portTICK_PERIOD_MS);
	}
	reading /= numReadings;

	test->log("reading", reading);

	if(reading < VoltReference - VoltReferenceTolerance || reading > VoltReference + VoltReferenceTolerance){
		return false;
	}

	gpio_set_level(RefSwitch, 0);
	delayMillis(100);

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
	const auto cX = canvas->getCursorX();
	const auto cY = canvas->getCursorY();
	bool flash = false;
	uint32_t flashTime = 0;

	std::vector<bool> pressed(ButtonCount, false);
	std::vector<bool> released(ButtonCount, false);
	uint8_t pressCount = 0;
	uint8_t releaseCount = 0;
	for(;;){
		for(int i = 0; i < ButtonCount; i++){
			if(input->isPressed((Input::Button) i) && !pressed[i]){
				pressed[i] = true;
				pressCount++;
			}else if(!input->isPressed((Input::Button) i) && pressed[i] && !released[i]){
				released[i] = true;
				releaseCount++;
			}
		}

		if(pressCount == ButtonCount && releaseCount == ButtonCount) break;

		if(millis() - flashTime > 500){
			if(flash){
				canvas->fillRect(cX, cY - 4, 120, 8, TFT_BLACK);
			}else{
				canvas->setCursor(cX, cY);
				canvas->setTextColor(TFT_GOLD);
				canvas->printf("-PRESS BTNS-");
			}

			canvas->pushSprite(0, 0);
			flash = !flash;
			flashTime = millis();
		}

		canvas->fillRect(cX, cY + 6, 120, 8, TFT_BLACK);
		canvas->setTextColor(TFT_LIGHTGRAY);
		canvas->setCursor(cX - 3, cY + 10);
		canvas->printf("[");
		for(int i = 0; i < ButtonCount; i++){
			if(input->isPressed((Input::Button) i)){
				canvas->setTextColor(TFT_GOLD);
			}else if(pressed[i] && released[i]){
				canvas->setTextColor(TFT_BLUE);
			}else{
				canvas->setTextColor(TFT_DARKGRAY);
			}
			canvas->printf("-");
		}
		canvas->setTextColor(TFT_LIGHTGRAY);
		canvas->printf("]");
		canvas->pushSprite(0, 0);

		delayMillis(10);
	}

	canvas->fillRect(cX - 3, cY - 4, 120, 20, TFT_BLACK);
	canvas->setCursor(cX, cY);
	return pressCount == ButtonCount && releaseCount == ButtonCount;
}

bool JigHWTest::Robot(){
	Robots rob;
	gpio_set_direction((gpio_num_t) Pins::get(Pin::Ctrl1), GPIO_MODE_OUTPUT);
	gpio_set_level((gpio_num_t) Pins::get(Pin::Ctrl1), 0);

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
			gpio_set_level((gpio_num_t) Pins::get(Pin::Ctrl1), 1);
			delayMillis(100);
			gpio_set_level((gpio_num_t) Pins::get(Pin::Ctrl1), 0);
			delayMillis(100);
		}
		delayMillis(1000);
	}, "Blinker", 2048);

	PWM buzzPwm(Pins::get(Pin::Buzz), LEDC_CHANNEL_0);
	ChirpSystem audio(buzzPwm);
	const auto buzz = [&audio](){
		audio.play({ Chirp{ 200, 200, 100 }});
	};
	const auto buzzDbl = [&audio](){
		audio.play({
						   Chirp{ 200, 200, 100 },
						   Chirp{ 0, 0, 100 },
						   Chirp{ 200, 200, 100 }
				   });
	};

	EventQueue evts(12);
	Events::listen(Facility::Robots, &evts);

	const auto out = [&evts, &blinker](){
		Events::unlisten(&evts);
		blinker.stop();
		gpio_set_level((gpio_num_t) Pins::get(Pin::Ctrl1), 0);
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

bool JigHWTest::HWVersion(){
	uint16_t pid = 1;
	bool result = EfuseMeta::readPID(pid);

	if(!result){
		test->log("HW version", "couldn't read from efuse");
		return false;
	}

	if(pid != 0){
		test->log("Existing HW version", (uint32_t) pid);
		if(pid == EfuseMeta::getHardcodedPID()){
			test->log("Already fused.", (uint32_t) pid);
			return true;
		}else{
			test->log("Wrong binary already fused!", (uint32_t) pid);
			return false;
		}
	}

	return EfuseMeta::write();
}
