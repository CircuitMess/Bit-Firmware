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
	inline static constexpr const char* Paused = "S:/Paused.bin";
	inline static constexpr const char* Settings = "S:/Settings.bin";
	inline static constexpr const char* Splash = "/spiffs/Splash.bmp";

	namespace Theme1 {
		inline static constexpr const char* Background = "S:/Theme1/bg.bin"; // TODO when theme 1 isn't active, make sure to not have the background animation of theme 1, but just background of the active theme
		inline static constexpr const char* Popup = "S:/Theme1/popup.bin";
		inline static constexpr const char* Header = "S:/Theme1/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme1/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme1/barShort.bin";
	}

	namespace Theme2 {
		inline static constexpr const char* Background = "S:/Theme2/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme2/popup.bin";
		inline static constexpr const char* Header = "S:/Theme2/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme2/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme2/barShort.bin";
	}

	namespace Theme3 {
		inline static constexpr const char* Background = "S:/Theme3/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme3/popup.bin";
		inline static constexpr const char* Header = "S:/Theme3/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme3/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme3/barShort.bin";
	}

	namespace Theme4 {
		inline static constexpr const char* Background = "S:/Theme4/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme4/popup.bin";
		inline static constexpr const char* Header = "S:/Theme4/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme4/barLong.bin";
		inline static constexpr const char* BarShort = "S:/Theme4/barShort.bin";
	}

	namespace Battery {
		inline static constexpr const char* EmptyBig = "S:/Batt/EmptyBig.bin";
		inline static constexpr const char* Low = "S:/Batt/Low.bin";
		inline static constexpr const char* Mid = "S:/Batt/Mid.bin";
		inline static constexpr const char* Full = "S:/Batt/Full.bin";
	}

	namespace Award {

	}

	namespace Profile {

	}

	namespace Achievement {

	}
}

#endif //BIT_FIRMWARE_FILEPATHS_HPP