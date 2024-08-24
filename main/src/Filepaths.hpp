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
		inline static constexpr const char* BackgroundSmall = "S:/Award/BG-small.bin";
		inline static constexpr const char* LeveledUp = "S:/Award/leveledup.bin";
		inline static constexpr const char* AchievementsUnlocked = "S:/Award/achievements.bin";
		inline static constexpr const char* XpFrame = "S:/Award/XP-frame.bin";
		inline static constexpr const char* XpBackground = "S:/Award/XP-line.bin";
		inline static constexpr const char* XpGained = "S:/Award/xpgained.bin";
	}

	namespace Profile {

	}
}

constexpr const char* AchievementFiles[] = {
		"S:/Achi/artemis-friendly.bin",
		"S:/Achi/artemis-sharp.bin",
		"S:/Achi/bee-b.bin",
		"S:/Achi/bee-crow.bin",
		"S:/Achi/bee-g.bin",
		"S:/Achi/bee-s.bin",
		"S:/Achi/blocks-4lines.bin",
		"S:/Achi/blocks-b.bin",
		"S:/Achi/blocks-clear.bin",
		"S:/Achi/blocks-g.bin",
		"S:/Achi/blocks-s.bin",
		"S:/Achi/bob-bombs.bin",
		"S:/Achi/bob-green.bin",
		"S:/Achi/bob-yellow.bin",
		"S:/Achi/bonk-5.bin",
		"S:/Achi/bonk-comeback.bin",
		"S:/Achi/buttons-triple.bin",
		"S:/Achi/buttons-win.bin",
		"S:/Achi/capacitron-b.bin",
		"S:/Achi/capacitron-fire.bin",
		"S:/Achi/capacitron-g.bin",
		"S:/Achi/capacitron-s.bin",
		"S:/Achi/charlie-nomercy.bin",
		"S:/Achi/charlie-yesmercy.bin",
		"S:/Achi/dusty-rat.bin",
		"S:/Achi/dusty-shovel.bin",
		"S:/Achi/fred-copper.bin",
		"S:/Achi/fred-detective.bin",
		"S:/Achi/fred-veteran.bin",
		"S:/Achi/hertz-3.bin",
		"S:/Achi/marv-5.bin",
		"S:/Achi/marv-life.bin",
		"S:/Achi/marv-newspapers.bin",
		"S:/Achi/planck-b.bin",
		"S:/Achi/planck-chauffeur.bin",
		"S:/Achi/planck-g.bin",
		"S:/Achi/planck-juice.bin",
		"S:/Achi/planck-nobrake.bin",
		"S:/Achi/planck-s.bin",
		"S:/Achi/resistron-b.bin",
		"S:/Achi/resistron-deflect.bin",
		"S:/Achi/resistron-g.bin",
		"S:/Achi/resistron-s.bin",
		"S:/Achi/robby-asteroid.bin",
		"S:/Achi/robby-sharp.bin",
		"S:/Achi/snake-b.bin",
		"S:/Achi/snake-fill.bin",
		"S:/Achi/snake-g.bin",
		"S:/Achi/snake-s.bin",
		"S:/Achi/sparkly-nobrake.bin",
		"S:/Achi/sparkly-overtake.bin",
		"S:/Achi/sparkly-road.bin",
		"S:/Achi/stacky-5.bin",
		"S:/Achi/stacky-highrise.bin",
		"S:/Achi/stacky-skyscrapper.bin",
		"S:/Achi/stacky-super.bin"
};

constexpr const char* AchievementFilesBW[] = {
		"S:/Achi/BW/artemis-friendly.bin",
		"S:/Achi/BW/artemis-sharp.bin",
		"S:/Achi/BW/bee-b.bin",
		"S:/Achi/BW/bee-crow.bin",
		"S:/Achi/BW/bee-g.bin",
		"S:/Achi/BW/bee-s.bin",
		"S:/Achi/BW/blocks-4lines.bin",
		"S:/Achi/BW/blocks-b.bin",
		"S:/Achi/BW/blocks-clear.bin",
		"S:/Achi/BW/blocks-g.bin",
		"S:/Achi/BW/blocks-s.bin",
		"S:/Achi/BW/bob-bombs.bin",
		"S:/Achi/BW/bob-green.bin",
		"S:/Achi/BW/bob-yellow.bin",
		"S:/Achi/BW/bonk-5.bin",
		"S:/Achi/BW/bonk-comeback.bin",
		"S:/Achi/BW/buttons-triple.bin",
		"S:/Achi/BW/buttons-win.bin",
		"S:/Achi/BW/capacitron-b.bin",
		"S:/Achi/BW/capacitron-fire.bin",
		"S:/Achi/BW/capacitron-g.bin",
		"S:/Achi/BW/capacitron-s.bin",
		"S:/Achi/BW/charlie-nomercy.bin",
		"S:/Achi/BW/charlie-yesmercy.bin",
		"S:/Achi/BW/dusty-rat.bin",
		"S:/Achi/BW/dusty-shovel.bin",
		"S:/Achi/BW/fred-copper.bin",
		"S:/Achi/BW/fred-detective.bin",
		"S:/Achi/BW/fred-veteran.bin",
		"S:/Achi/BW/hertz-3.bin",
		"S:/Achi/BW/marv-5.bin",
		"S:/Achi/BW/marv-life.bin",
		"S:/Achi/BW/marv-newspapers.bin",
		"S:/Achi/BW/planck-b.bin",
		"S:/Achi/BW/planck-chauffeur.bin",
		"S:/Achi/BW/planck-g.bin",
		"S:/Achi/BW/planck-juice.bin",
		"S:/Achi/BW/planck-nobrake.bin",
		"S:/Achi/BW/planck-s.bin",
		"S:/Achi/BW/resistron-b.bin",
		"S:/Achi/BW/resistron-deflect.bin",
		"S:/Achi/BW/resistron-g.bin",
		"S:/Achi/BW/resistron-s.bin",
		"S:/Achi/BW/robby-asteroid.bin",
		"S:/Achi/BW/robby-sharp.bin",
		"S:/Achi/BW/snake-b.bin",
		"S:/Achi/BW/snake-fill.bin",
		"S:/Achi/BW/snake-g.bin",
		"S:/Achi/BW/snake-s.bin",
		"S:/Achi/BW/sparkly-nobrake.bin",
		"S:/Achi/BW/sparkly-overtake.bin",
		"S:/Achi/BW/sparkly-road.bin",
		"S:/Achi/BW/stacky-5.bin",
		"S:/Achi/BW/stacky-highrise.bin",
		"S:/Achi/BW/stacky-skyscrapper.bin",
		"S:/Achi/BW/stacky-super.bin"
};

constexpr const char* AchivementFile(Achievement achievement){
	return AchievementFiles[(uint32_t) achievement];
}

constexpr const char* AchivementFileBW(Achievement achievement){
	return AchievementFilesBW[(uint32_t) achievement];
}

#endif //BIT_FIRMWARE_FILEPATHS_HPP