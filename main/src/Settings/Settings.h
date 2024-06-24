#ifndef BIT_FIRMWARE_SETTINGS_H
#define BIT_FIRMWARE_SETTINGS_H

#include <nvs.h>
#include "Services/Robots.h"

enum class Theme : uint8_t {
	Theme1,
	Theme2,
	Theme3,
	Theme4,
	COUNT
};

#define THEMED_COLOR(Color, theme)     \
    lv_color_hex(\
    theme == Theme::Theme1 ? ThemeColor::Theme1::Color :         \
    theme == Theme::Theme2 ? ThemeColor::Theme2::Color :         \
    theme == Theme::Theme3 ? ThemeColor::Theme3::Color :         \
    theme == Theme::Theme4 ? ThemeColor::Theme4::Color : 0       \
    )

namespace ThemeColor {

	namespace Theme1 {
		inline static constexpr uint32_t MenuHeaderText = 0x4b5f6b;
		inline static constexpr uint32_t PausedForeground = 0xcfc6b8;
		inline static constexpr uint32_t HighlightPrimary = 0xd999ba;
		inline static constexpr uint32_t HighlightSecondary = 0xa34578;
	}

	namespace Theme2 {
		inline static constexpr uint32_t MenuHeaderText = 0x3f1d37;
		inline static constexpr uint32_t PausedForeground = 0xcfc6b8;
		inline static constexpr uint32_t HighlightPrimary = 0x97e9d1;
		inline static constexpr uint32_t HighlightSecondary = 0x197f96;
	}

	namespace Theme3 {
		inline static constexpr uint32_t MenuHeaderText = 0xffb132;
		inline static constexpr uint32_t PausedForeground = 0xffffff;
		inline static constexpr uint32_t HighlightPrimary = 0xffb132;
		inline static constexpr uint32_t HighlightSecondary = 0xfb4831;
	}

	namespace Theme4 {
		inline static constexpr uint32_t MenuHeaderText = 0x264f6e;
		inline static constexpr uint32_t PausedForeground = 0xffffff;
		inline static constexpr uint32_t HighlightPrimary = 0x449642;
		inline static constexpr uint32_t HighlightSecondary = 0x1a293c;
	}

}

struct __SettingsStruct_Old { // NOLINT(*-reserved-identifier)
	bool sound = true;
	uint8_t screenBrightness = 100;
	uint8_t sleepTime = 3;
	uint8_t robotsSeen[9] = {0};
};

struct SettingsStruct {
	bool sound = true;
	uint8_t screenBrightness = 100;
	uint8_t sleepTime = 3;
	Theme theme = Theme::Theme1;
	uint8_t avatar = 0;
	int8_t pet = -1;
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
