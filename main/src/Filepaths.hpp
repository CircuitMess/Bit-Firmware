#ifndef BIT_FIRMWARE_FILEPATHS_HPP
#define BIT_FIRMWARE_FILEPATHS_HPP

#include "Services/AchievementSystem.h"

#define THEMED_FILE(Filename, theme) 									\
	theme == Theme::Theme1 ? Filepath::Theme1::Filename : 				\
	theme == Theme::Theme2 ? Filepath::Theme2::Filename : 				\
	theme == Theme::Theme3 ? Filepath::Theme3::Filename : 				\
	theme == Theme::Theme4 ? Filepath::Theme4::Filename : ""			\

namespace Filepath {
	// These might change to be consistent with the active theme
	inline static constexpr const char* BackgroundSplash = "/spiffs/bgSplash.bmp";
	inline static constexpr const char* Locked = "S:/Lock.bin";
	inline static constexpr const char* Unlocked = "S:/Lock_unlocked.bin";
	inline static constexpr const char* RobotPopup = "S:/ModalBg2.bin";
	inline static constexpr const char* Splash = "/spiffs/Splash.bmp";
	inline static constexpr const char* SplashWithBackground = "/spiffs/bgSplash.bmp";
	inline static constexpr const char* Modal = "S:/ModalBg.bin";

	namespace Theme1 {
		inline static constexpr const char* Background = "S:/Theme1/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme1/popup.bin";
		inline static constexpr const char* Header = "S:/Theme1/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme1/barLong.bin";
		inline static constexpr const char* BarMedium = "S:/Theme1/barMid.bin";
		inline static constexpr const char* BarShort = "S:/Theme1/barShort.bin";
		inline static constexpr const char* Paused = "S:/Pause/title1.bin";
		inline static constexpr const char* PausedBg = "S:/Pause/bg1.bin";
		inline static constexpr const char* Settings = "S:/Theme1/settings.bin";

		namespace Battery {
			inline static constexpr const char* Background = "S:/Theme1/battery/bg.bin";
			inline static constexpr const char* Level1 = "S:/Theme1/battery/1.bin";
			inline static constexpr const char* Level2 = "S:/Theme1/battery/2.bin";
			inline static constexpr const char* Level3 = "S:/Theme1/battery/3.bin";
			inline static constexpr const char* Level4 = "S:/Theme1/battery/4.bin";
			inline static constexpr const char* Level5 = "S:/Theme1/battery/5.bin";
			inline static constexpr const char* Level6 = "S:/Theme1/battery/6.bin";
		}
	}

	namespace Theme2 {
		inline static constexpr const char* Background = "S:/Theme2/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme2/popup.bin";
		inline static constexpr const char* Header = "S:/Theme2/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme2/barLong.bin";
		inline static constexpr const char* BarMedium = "S:/Theme2/barMid.bin";
		inline static constexpr const char* BarShort = "S:/Theme2/barShort.bin";
		inline static constexpr const char* Paused = "S:/Pause/title2.bin";
		inline static constexpr const char* PausedBg = "S:/Pause/bg2.bin";
		inline static constexpr const char* Settings = "S:/Theme2/settings.bin";

		namespace Battery {
			inline static constexpr const char* Background = "S:/Theme2/battery/bg.bin";
			inline static constexpr const char* Level1 = "S:/Theme2/battery/1.bin";
			inline static constexpr const char* Level2 = "S:/Theme2/battery/2.bin";
			inline static constexpr const char* Level3 = "S:/Theme2/battery/3.bin";
			inline static constexpr const char* Level4 = "S:/Theme2/battery/4.bin";
			inline static constexpr const char* Level5 = "S:/Theme2/battery/5.bin";
			inline static constexpr const char* Level6 = "S:/Theme2/battery/6.bin";
		}
	}

	namespace Theme3 {
		inline static constexpr const char* Background = "S:/Theme3/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme3/popup.bin";
		inline static constexpr const char* Header = "S:/Theme3/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme3/barLong.bin";
		inline static constexpr const char* BarMedium = "S:/Theme3/barMid.bin";
		inline static constexpr const char* BarShort = "S:/Theme3/barShort.bin";
		inline static constexpr const char* Paused = "S:/Pause/title3.bin";
		inline static constexpr const char* PausedBg = "S:/Pause/bg3.bin";
		inline static constexpr const char* Settings = "S:/Theme3/settings.bin";

		namespace Battery {
			inline static constexpr const char* Background = "S:/Theme3/battery/bg.bin";
			inline static constexpr const char* Level1 = "S:/Theme3/battery/1.bin";
			inline static constexpr const char* Level2 = "S:/Theme3/battery/2.bin";
			inline static constexpr const char* Level3 = "S:/Theme3/battery/3.bin";
			inline static constexpr const char* Level4 = "S:/Theme3/battery/4.bin";
			inline static constexpr const char* Level5 = "S:/Theme3/battery/5.bin";
			inline static constexpr const char* Level6 = "S:/Theme3/battery/6.bin";
		}
	}

	namespace Theme4 {
		inline static constexpr const char* Background = "S:/Theme4/bg.bin";
		inline static constexpr const char* Popup = "S:/Theme4/popup.bin";
		inline static constexpr const char* Header = "S:/Theme4/header.bin";
		inline static constexpr const char* BarLong = "S:/Theme4/barLong.bin";
		inline static constexpr const char* BarMedium = "S:/Theme4/barMid.bin";
		inline static constexpr const char* BarShort = "S:/Theme4/barShort.bin";
		inline static constexpr const char* Paused = "S:/Pause/title4.bin";
		inline static constexpr const char* PausedBg = "S:/Pause/bg4.bin";
		inline static constexpr const char* Settings = "S:/Theme4/settings.bin";

		namespace Battery {
			inline static constexpr const char* Background = "S:/Theme4/battery/bg.bin";
			inline static constexpr const char* Level1 = "S:/Theme4/battery/1.bin";
			inline static constexpr const char* Level2 = "S:/Theme4/battery/2.bin";
			inline static constexpr const char* Level3 = "S:/Theme4/battery/3.bin";
			inline static constexpr const char* Level4 = "S:/Theme4/battery/4.bin";
			inline static constexpr const char* Level5 = "S:/Theme4/battery/5.bin";
			inline static constexpr const char* Level6 = "S:/Theme4/battery/6.bin";
		}
	}

	namespace Battery {
		inline static constexpr const char* EmptyBig = "S:/Batt/EmptyBig.bin";
	}

	namespace Award {
		inline static constexpr const char* LeveledUp = "S:/Award/leveledup.bin";
		inline static constexpr const char* AchievementsUnlocked = "S:/Award/achievements.bin";
		inline static constexpr const char* XpFrame = "S:/Award/XP-frame.bin";
		inline static constexpr const char* XpBackground = "S:/Award/XP-line.bin";
		inline static constexpr const char* XpGained = "S:/Award/xpgained.bin";
	}

	namespace Profile {

	}
}

struct AchiFile {
	const char* unlocked;
	const char* locked;
};

extern const std::unordered_map<Achievement, AchiFile> AchievementFiles;

const char* AchivementFile(Achievement achievement);

const char* AchivementFileBW(Achievement achievement);

#endif //BIT_FIRMWARE_FILEPATHS_HPP