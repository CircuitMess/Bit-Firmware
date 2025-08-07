#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/PWM.h"
#include "Pins.hpp"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
#include "Services/Sleep.h"
#include "Periph/I2C.h"
#include "Devices/Display.h"
#include "Devices/Input.h"
#include "Devices/Battery.h"
#include "Devices/Battery/BatteryRev3.h"
#include "Devices/Battery/BatteryRev1.h"
#include "Util/Notes.h"
#include "FS/SPIFFS.h"
#include "UIThread.h"
#include "Services/Robots.h"
#include "Services/RobotManager.h"
#include "LV_Interface/LVGL.h"
#include "LV_Interface/InputLVGL.h"
#include "LV_Interface/FSLVGL.h"
#include <esp_sleep.h>
#include <Util/stdafx.h>
#include "JigHWTest/JigHWTest.h"
#include "Periph/NVSFlash.h"
#include "Services/XPSystem.h"
#include "Services/AchievementSystem.h"
#include "Services/HighScoreManager.h"
#include "Services/SystemManager.h"
#include "Filepaths.hpp"
#include "NVSUpgrades/NVSUpgrades.h"
#include "Screens/MainMenu/MainMenu.h"
#include "driver/rtc_io.h"
#include "Services/LEDService.h"
#include "Services/Allocator.h"
#include "Util/EfuseMeta.h"
BacklightBrightness* bl;

void shutdown(){
	bl->fadeOut();

	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RC_FAST, ESP_PD_OPTION_AUTO);
	esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_AUTO);
	esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_AUTO);
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);

	//PIN_BL will be held high, since that is the last state set by bl->fadeOut()
	//Required to prevent MOSFET activation on TFT_BL with leaked current if pin is floating

	const auto blPin = (gpio_num_t)Pins::get(Pin::LedBl);
	gpio_set_direction(blPin, GPIO_MODE_OUTPUT);
	gpio_set_level(blPin, true);
	gpio_set_pull_mode(blPin, GPIO_PULLUP_ONLY);
	gpio_hold_en(blPin);

	const auto buzzPin = (gpio_num_t)Pins::get(Pin::Buzz);
	gpio_set_direction(buzzPin, GPIO_MODE_OUTPUT);
	gpio_set_level(buzzPin, false);
	gpio_set_pull_mode(buzzPin, GPIO_PULLDOWN_ONLY);
	gpio_hold_en(buzzPin);

	const auto calibEn = (gpio_num_t)Pins::get(Pin::CalibVrefEn);
	gpio_set_direction(calibEn, GPIO_MODE_OUTPUT);
	gpio_set_level(calibEn, false);
	gpio_set_pull_mode(calibEn, GPIO_PULLDOWN_ONLY);
	gpio_hold_en(calibEn);


	gpio_deep_sleep_hold_en();
	esp_deep_sleep_start();
}

/**
 * No need to check Clockstar v2 - Bit v3 PID conflict here.
 * Bit Firmware only fits into 8MB flash, so uploading it to Clockstar's 4MB isn't possible.
 */

void init(){
	if(!EfuseMeta::check()){
		while(true){
			vTaskDelay(1000);
			EfuseMeta::log();
		}
	}

	auto alloc = new Allocator(86 * 1024);

	auto nvs = new NVSFlash();
	Services.set(Service::NVS, nvs);

	new SystemManager(NVSUpgrades);

	auto settings = new Settings();
	Services.set(Service::Settings, settings);

	if(JigHWTest::checkJig()){
		printf("Jig\n");

		Pins::setLatest();

		auto set = settings->get();
		set.sound = true;
		settings->set(set);

		auto test = new JigHWTest();
		test->start();
		vTaskDelete(nullptr);
	}else{
		printf("Hello\n");
	}

	gpio_config_t cfg = {
			.pin_bit_mask = (1ULL << Pins::get(Pin::I2cSda)) | (1ULL << Pins::get(Pin::I2cScl)),
			.mode = GPIO_MODE_INPUT
	};
	gpio_config(&cfg);

	auto xpsystem = new XPSystem();
	Services.set(Service::XPSystem, xpsystem);

	auto achievements = new AchievementSystem();
	Services.set(Service::Achievements, achievements);

	auto blPwm = new PWM(Pins::get(Pin::LedBl), LEDC_CHANNEL_1, true);
	blPwm->detach();
	bl = new BacklightBrightness(blPwm);
	Services.set(Service::Backlight, bl);

	auto adc1 = new ADC(ADC_UNIT_1);

	uint8_t revision = 0;
	EfuseMeta::readRev(revision);

	Battery* battery;
	if(revision == 3){
		battery = new BatteryRev3(*adc1);
	}else{
		battery = new BatteryRev1(*adc1);
	}

	if(battery->isShutdown()){
		shutdown();
		return;
	}
	Services.set(Service::Battery, battery);

	auto led = new LEDService();
	Services.set(Service::LED, led);

	if(!SPIFFS::init()) return;

	auto disp = new Display(revision);
	Services.set(Service::Display, disp);

	disp->getLGFX().drawBmpFile(Filepath::Splash, 36, 11);
	bl->fadeIn();
	auto splashStart = millis();

	auto buzzPwm = new PWM(Pins::get(Pin::Buzz), LEDC_CHANNEL_0);
	auto audio = new ChirpSystem(*buzzPwm);
	Services.set(Service::Audio, audio);

	auto input = new Input(true);
	Services.set(Service::Input, input);

	auto sleep = new Sleep();

	// GameManager before robot detector, in case robot is plugged in during boot
	auto games = new RobotManager();
	Services.set(Service::RobotManager, games);
	auto highScore = new HighScoreManager();
	Services.set(Service::HighScore, highScore);
	auto rob = new Robots();
	Services.set(Service::Robots, rob);

	auto lvgl = new LVGL(*disp);
	auto lvInput = new InputLVGL();
	auto lvFS = new FSLVGL('S', alloc);

	auto gamer = new GameRunner(*disp, alloc);

	if(settings->get().sound){
		audio->play({
				Chirp { NOTE_D4, NOTE_A4, 100 },
				Chirp { 0, 0, 50 },
				Chirp { NOTE_D5, NOTE_D5, 100 },
				Chirp { 0, 0, 50 },
				Chirp { NOTE_A4, NOTE_A4, 100 },
				Chirp { 0, 0, 50 },
				Chirp { NOTE_G5, NOTE_G4, 100 },
				Chirp { 0, 0, 50 },
				Chirp { NOTE_D4, NOTE_A4, 100 },
				Chirp { NOTE_D5, NOTE_D5, 100 },
		});
	}

	lvFS->loadCache();

	auto ui = new UIThread(*lvgl, *gamer, *lvFS);
	Services.set(Service::UI, ui);

	while(millis() - splashStart < 2000){
		delayMillis(10);
	}

	bl->fadeOut();
	ui->start();
	ui->startScreen([](){ return std::make_unique<MainMenu>(true); });
	delayMillis(200);
	bl->fadeIn();

	// Start Battery scanning after everything else, otherwise Critical
	// Battery event might come while initialization is still in progress
	battery->begin();
}

extern "C" void app_main(void){
	init();

	vTaskDelete(nullptr);
}
