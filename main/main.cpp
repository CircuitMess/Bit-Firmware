#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/PWM.h"
#include "Pins.hpp"
#include "Services/BacklightBrightness.h"
#include "Services/ChirpSystem.h"
#include "Periph/I2C.h"
#include "Devices/Display.h"
#include "Devices/Input.h"
#include "Devices/Battery.h"
#include "Util/Notes.h"
#include <esp_spiffs.h>
#include "UIThread.h"
#include "Games/TestGame.h"

BacklightBrightness* bl;

bool initSPIFFS(){
	esp_vfs_spiffs_conf_t conf = {
			.base_path = "/spiffs",
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};

	auto ret = esp_vfs_spiffs_register(&conf);
	if(ret != ESP_OK){
		if(ret == ESP_FAIL){
			ESP_LOGE("FS", "Failed to mount or format filesystem");
		}else if(ret == ESP_ERR_NOT_FOUND){
			ESP_LOGE("FS", "Failed to find SPIFFS partition");
		}else{
			ESP_LOGE("FS", "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
		}
		return false;
	}

	return true;
}

void init(){
	auto ret = nvs_flash_init();
	if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND){
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	if(!initSPIFFS()) return;

	auto settings = new Settings();
	Services.set(Service::Settings, settings);

	auto blPwm = new PWM(PIN_BL, LEDC_CHANNEL_1, true);
	blPwm->detach();
	bl = new BacklightBrightness(blPwm);
	Services.set(Service::Backlight, bl);

	auto buzzPwm = new PWM(PIN_BUZZ, LEDC_CHANNEL_0);
	auto audio = new ChirpSystem(*buzzPwm);
	Services.set(Service::Audio, audio);

	auto i2c = new I2C(I2C_NUM_0, (gpio_num_t) I2C_SDA, (gpio_num_t) I2C_SCL);

	auto disp = new Display();
	auto input = new Input();
	Services.set(Service::Input, input);

	auto battery = new Battery(); // Battery is doing shutdown
	if(battery->isShutdown()) return; // Stop initialization if battery is critical
	Services.set(Service::Battery, battery);

	if(settings->get().sound){
		//TODO - startup chime
	}

	bl->fadeIn();

	// Start Battery scanning after everything else, otherwise Critical
	// Battery event might come while initialization is still in progress
	battery->begin();

	auto ui = new UIThread(*disp);
	ui->startGame(std::make_unique<TestGame>(disp->getCanvas()));
}

extern "C" void app_main(void){
	init();

	vTaskDelete(nullptr);
}
