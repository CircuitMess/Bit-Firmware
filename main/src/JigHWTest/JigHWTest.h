#ifndef BIT_FIRMWARE_JIGHWTEST_H
#define BIT_FIRMWARE_JIGHWTEST_H

#include <vector>
#include "Devices/Display.h"
#include "Util/stdafx.h"
#include "Devices/Battery/BatteryRev3.h"
#include <esp_efuse.h>
#include <esp_spiffs.h>
#include "Periph/I2C.h"
#include <Pins.hpp>
#include "Services/Robots.h"
#include "Devices/Input.h"

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
	static LGFX_Device* panel;
	static LGFX_Sprite* canvas;
	static JigHWTest* test;
	static Input* input;
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

	static bool BatteryCheck();
	static bool VoltReferenceCheck();
	static bool Robot();
	static bool SPIFFSTest();
	static bool Buttons();
	static bool HWVersion();

	static uint32_t calcChecksum(FILE* file);

	gpio_num_t led_pin = (gpio_num_t) Pins::get(Pin::Ctrl1);

	void rgb();


	static constexpr int16_t BatVoltageMinimum = 4400;
	static constexpr float VoltReference = 2500;
	static constexpr float VoltReferenceTolerance = 100;

	static constexpr uint32_t CheckTimeout = 500;

	static constexpr esp_vfs_spiffs_conf_t spiffsConfig = {
			.base_path = "/spiffs",
			.partition_label = "storage",
			.max_files = 8,
			.format_if_mount_failed = false
	};

	static constexpr uint8_t ButtonCount = 7;
};

#endif //BIT_FIRMWARE_JIGHWTEST_H
