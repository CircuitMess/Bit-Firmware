#ifndef BIT_FIRMWARE_SETTINGS_H
#define BIT_FIRMWARE_SETTINGS_H

#include <nvs.h>
#include "Services/Robots.h"

enum class Theme : uint8_t {
	Theme1,
	Theme2,
	Theme3,
	Theme4
};

// TODO need to make a migration when loading this from spiffs
struct __SettingsStruct_Old { // NOLINT(*-reserved-identifier)
	bool sound = true;
	uint8_t screenBrightness = 100;
	uint8_t sleepTime = 3;
	uint8_t robotsSeen[Robot::COUNT] = {0};
};

struct SettingsStruct {
	bool sound = true;
	uint8_t screenBrightness = 100;
	uint8_t sleepTime = 3;
	uint8_t robotsSeen[Robot::COUNT] = {0};
	Theme theme = Theme::Theme1;
};

class Settings {
public:
	Settings();

	SettingsStruct get() const;
	void set(SettingsStruct& settings);
	void store();

	static constexpr uint8_t SleepSteps = 5;
	static constexpr const uint32_t SleepSeconds[SleepSteps] = { 0, 30, 60, 2 * 60, 5 * 60 };
	static constexpr const char* SleepText[SleepSteps] = { "OFF", "30 sec", "1 min", "2 min", "5 min" };

private:
	SettingsStruct settingsStruct;

	static constexpr const char* BlobName = "Settings";

	void load();
};


#endif //BIT_FIRMWARE_SETTINGS_H
