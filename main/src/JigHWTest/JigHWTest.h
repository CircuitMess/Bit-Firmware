#ifndef BIT_FIRMWARE_JIGHWTEST_H
#define BIT_FIRMWARE_JIGHWTEST_H

#include <vector>
#include "Devices/Display.h"
#include "Util/stdafx.h"
#include "Devices/Battery.h"
#include <esp_efuse.h>
#include <esp_spiffs.h>
#include "Periph/I2C.h"
#include <Pins.hpp>
#include "Services/Robots.h"

struct Test {
	bool (* test)();
	const char* name;
	void (* onFail)();
};

class JigHWTest {
public:
	JigHWTest();
	static bool checkJig();
	void start();

private:
	static Display* display;
	static LGFX_Device* canvas;
	static JigHWTest* test;
	std::vector<Test> tests;
	const char* currentTest;

	void log(const char* property, const char* value);
	void log(const char* property, float value);
	void log(const char* property, double value);
	void log(const char* property, bool value);
	void log(const char* property, uint32_t value);
	void log(const char* property, int32_t value);
	void log(const char* property, const std::string& value);

	void instr(const char* msg);

	static bool Robot();
	static bool SPIFFSTest();
	static bool BatteryCalib();
	static bool BatteryCheck();
	static bool Buttons();

	static uint32_t calcChecksum(FILE* file);

	static constexpr gpio_num_t led_pin = (gpio_num_t) CTRL_1;

	void rgb();


	static const int16_t referenceVoltage = 4000; // 50mV for backlight voltage drop compensation

	static constexpr uint32_t CheckTimeout = 500;

	static constexpr esp_efuse_desc_t adc1_low = { EFUSE_BLK3, 0, 8 };
	static constexpr const esp_efuse_desc_t* efuse_adc1_low[] = { &adc1_low, nullptr };
	static constexpr esp_efuse_desc_t adc1_high = { EFUSE_BLK3, 8, 8 };
	static constexpr const esp_efuse_desc_t* efuse_adc1_high[] = { &adc1_high, nullptr };

	static constexpr esp_vfs_spiffs_conf_t spiffsConfig = {
			.base_path = "/spiffs",
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};
};

#endif //BIT_FIRMWARE_JIGHWTEST_H
