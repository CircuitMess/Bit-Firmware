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
	inline static constexpr const char* IconBorder = "S:/IconBorder.bin";
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
		inline static constexpr const char* Paused = "S:/Theme1/paused.bin";
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
		inline static constexpr const char* Paused = "S:/Theme2/paused.bin";
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
		inline static constexpr const char* Paused = "S:/Theme3/paused.bin";
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
		inline static constexpr const char* Paused = "S:/Theme4/paused.bin";
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
		inline static constexpr const char* XpFrame = "S:/Award/XP-frame.bin";
		inline static constexpr const char* XpBackground = "S:/Award/XP-line.bin";
		inline static constexpr const char* XpGained = "S:/Award/xpgained.bin";
	}

	namespace Profile {

	}
}

constexpr const char* AchievementFiles[] = {
		"S:/Ach/artemis-friendly.bin",
		"S:/Ach/artemis-sharp.bin",
		"S:/Ach/bee-b.bin",
		"S:/Ach/bee-crow.bin",
		"S:/Ach/bee-g.bin",
		"S:/Ach/bee-s.bin",
		"S:/Ach/blocks-4lines.bin",
		"S:/Ach/blocks-b.bin",
		"S:/Ach/blocks-clear.bin",
		"S:/Ach/blocks-g.bin",
		"S:/Ach/blocks-s.bin",
		"S:/Ach/bob-bombs.bin",
		"S:/Ach/bob-green.bin",
		"S:/Ach/bob-yellow.bin",
		"S:/Ach/bonk-5.bin",
		"S:/Ach/bonk-comeback.bin",
		"S:/Ach/buttons-triple.bin",
		"S:/Ach/buttons-win.bin",
		"S:/Ach/capacitron-b.bin",
		"S:/Ach/capacitron-fire.bin",
		"S:/Ach/capacitron-g.bin",
		"S:/Ach/capacitron-s.bin",
		"S:/Ach/charlie-nomercy.bin",
		"S:/Ach/charlie-yesmercy.bin",
		"S:/Ach/dusty-rat.bin",
		"S:/Ach/dusty-shovel.bin",
		"S:/Ach/fred-copper.bin",
		"S:/Ach/fred-detective.bin",
		"S:/Ach/fred-veteran.bin",
		"S:/Ach/hertz-3.bin",
		"S:/Ach/marv-5.bin",
		"S:/Ach/marv-life.bin",
		"S:/Ach/marv-newspapers.bin",
		"S:/Ach/planck-b.bin",
		"S:/Ach/planck-chauffeur.bin",
		"S:/Ach/planck-g.bin",
		"S:/Ach/planck-juice.bin",
		"S:/Ach/planck-nobrake.bin",
		"S:/Ach/planck-s.bin",
		"S:/Ach/resistron-b.bin",
		"S:/Ach/resistron-deflect.bin",
		"S:/Ach/resistron-g.bin",
		"S:/Ach/resistron-s.bin",
		"S:/Ach/robby-asteroid.bin",
		"S:/Ach/robby-sharp.bin",
		"S:/Ach/snake-b.bin",
		"S:/Ach/snake-fill.bin",
		"S:/Ach/snake-g.bin",
		"S:/Ach/snake-s.bin",
		"S:/Ach/sparkly-nobrake.bin",
		"S:/Ach/sparkly-overtake.bin",
		"S:/Ach/sparkly-road.bin",
		"S:/Ach/stacky-5.bin",
		"S:/Ach/stacky-highrise.bin",
		"S:/Ach/stacky-skyscrapper.bin",
		"S:/Ach/stacky-super.bin"
};

constexpr const char* AchievementFilesBW[] = {
		"S:/Ach/bw/artemis-friendly.bin",
		"S:/Ach/bw/artemis-sharp.bin",
		"S:/Ach/bw/bee-b.bin",
		"S:/Ach/bw/bee-crow.bin",
		"S:/Ach/bw/bee-g.bin",
		"S:/Ach/bw/bee-s.bin",
		"S:/Ach/bw/blocks-4lines.bin",
		"S:/Ach/bw/blocks-b.bin",
		"S:/Ach/bw/blocks-clear.bin",
		"S:/Ach/bw/blocks-g.bin",
		"S:/Ach/bw/blocks-s.bin",
		"S:/Ach/bw/bob-bombs.bin",
		"S:/Ach/bw/bob-green.bin",
		"S:/Ach/bw/bob-yellow.bin",
		"S:/Ach/bw/bonk-5.bin",
		"S:/Ach/bw/bonk-comeback.bin",
		"S:/Ach/bw/buttons-triple.bin",
		"S:/Ach/bw/buttons-win.bin",
		"S:/Ach/bw/capacitron-b.bin",
		"S:/Ach/bw/capacitron-fire.bin",
		"S:/Ach/bw/capacitron-g.bin",
		"S:/Ach/bw/capacitron-s.bin",
		"S:/Ach/bw/charlie-nomercy.bin",
		"S:/Ach/bw/charlie-yesmercy.bin",
		"S:/Ach/bw/dusty-rat.bin",
		"S:/Ach/bw/dusty-shovel.bin",
		"S:/Ach/bw/fred-copper.bin",
		"S:/Ach/bw/fred-detective.bin",
		"S:/Ach/bw/fred-veteran.bin",
		"S:/Ach/bw/hertz-3.bin",
		"S:/Ach/bw/marv-5.bin",
		"S:/Ach/bw/marv-life.bin",
		"S:/Ach/bw/marv-newspapers.bin",
		"S:/Ach/bw/planck-b.bin",
		"S:/Ach/bw/planck-chauffeur.bin",
		"S:/Ach/bw/planck-g.bin",
		"S:/Ach/bw/planck-juice.bin",
		"S:/Ach/bw/planck-nobrake.bin",
		"S:/Ach/bw/planck-s.bin",
		"S:/Ach/bw/resistron-b.bin",
		"S:/Ach/bw/resistron-deflect.bin",
		"S:/Ach/bw/resistron-g.bin",
		"S:/Ach/bw/resistron-s.bin",
		"S:/Ach/bw/robby-asteroid.bin",
		"S:/Ach/bw/robby-sharp.bin",
		"S:/Ach/bw/snake-b.bin",
		"S:/Ach/bw/snake-fill.bin",
		"S:/Ach/bw/snake-g.bin",
		"S:/Ach/bw/snake-s.bin",
		"S:/Ach/bw/sparkly-nobrake.bin",
		"S:/Ach/bw/sparkly-overtake.bin",
		"S:/Ach/bw/sparkly-road.bin",
		"S:/Ach/bw/stacky-5.bin",
		"S:/Ach/bw/stacky-highrise.bin",
		"S:/Ach/bw/stacky-skyscrapper.bin",
		"S:/Ach/bw/stacky-super.bin"
};

constexpr const char* AchivementFile(Achievement achievement){
	return AchievementFiles[(uint32_t) achievement];
}

constexpr const char* AchivementFileBW(Achievement achievement){
	return AchievementFilesBW[(uint32_t) achievement];
}

#endif //BIT_FIRMWARE_FILEPATHS_HPP