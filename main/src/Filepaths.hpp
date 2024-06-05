#ifndef BIT_FIRMWARE_FILEPATHS_HPP
#define BIT_FIRMWARE_FILEPATHS_HPP

#define THEMED_FILE(Filename, theme) 									\
	theme == Theme::Theme1 ? Filepath::Theme1::Filename : 				\
	theme == Theme::Theme2 ? Filepath::Theme2::Filename : 				\
	theme == Theme::Theme3 ? Filepath::Theme3::Filename : 				\
	theme == Theme::Theme4 ? Filepath::Theme4::Filename : ""			\

namespace Filepath {
	// These might change to be consistent with the active theme
	inline static constexpr const char* BackgroundSplash = "/spiffs/bgSplash.bmp";
	inline static constexpr const char* IconBorder = "S:/IconBorder.bin";
	inline static constexpr const char* Locked = "S:/Lock.bin";
	inline static constexpr const char* Unlocked = "S:/Lock_unlocked.bin";
	inline static constexpr const char* RobotPopup = "S:/ModalBg2.bin";
	inline static constexpr const char* Splash = "/spiffs/Splash.bmp";

	namespace Theme1 {
		inline static constexpr const char* Background = "S:/Theme1/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme1/popup.bin";
		inline static constexpr const char* Header = "S:/Theme1/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme1/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme1/barShort.bin";
		inline static constexpr const char* Paused = "S:/Theme1/paused.bin";
		inline static constexpr const char* Settings = "S:/Theme1/settings.bin";
	}

	namespace Theme2 {
		inline static constexpr const char* Background = "S:/Theme2/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme2/popup.bin";
		inline static constexpr const char* Header = "S:/Theme2/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme2/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme2/barShort.bin";
		inline static constexpr const char* Paused = "S:/Theme2/paused.bin";
		inline static constexpr const char* Settings = "S:/Theme2/settings.bin";
	}

	namespace Theme3 {
		inline static constexpr const char* Background = "S:/Theme3/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme3/popup.bin";
		inline static constexpr const char* Header = "S:/Theme3/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme3/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme3/barShort.bin";
		inline static constexpr const char* Paused = "S:/Theme3/paused.bin";
		inline static constexpr const char* Settings = "S:/Theme3/settings.bin";
	}

	namespace Theme4 {
		inline static constexpr const char* Background = "S:/Theme4/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme4/popup.bin";
		inline static constexpr const char* Header = "S:/Theme4/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme4/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme4/barShort.bin";
		inline static constexpr const char* Paused = "S:/Theme4/paused.bin";
		inline static constexpr const char* Settings = "S:/Theme4/settings.bin";
	}

	namespace Battery {
		inline static constexpr const char* EmptyBig = "S:/Batt/EmptyBig.bin";
		inline static constexpr const char* Low = "S:/Batt/Low.bin";
		inline static constexpr const char* Mid = "S:/Batt/Mid.bin";
		inline static constexpr const char* Full = "S:/Batt/Full.bin";
	}

	namespace Award {
		inline static constexpr const char* BackgroundSmall = "S:/Award/BG-small.bin";
		inline static constexpr const char* LeveledUp = "S:/Award/leveledup.bin";
		inline static constexpr const char* XpFrame = "S:/Award/XP-frame.bin";
		inline static constexpr const char* XpBackground = "S:/Award/XP-line.bin";
		inline static constexpr const char* XpGained = "S:/Award/xpgained.bin";
	}

	namespace Profile {

	}

	namespace Achievement {

	}
}

#endif //BIT_FIRMWARE_FILEPATHS_HPP