#include "Filepaths.hpp"

const std::unordered_map<Achievement, AchiFile> AchievementFiles = {
		{ Achievement::Artemis_friendly, { "S:/Achi/artemis-friendly.bin", "S:/Achi/BW/artemis-friendly.bin" }},
		{ Achievement::Artemis_sharp,    { "S:/Achi/artemis-sharp.bin",    "S:/Achi/BW/artemis-sharp.bin" }},
		{ Achievement::Bee_b,            { "S:/Achi/bee-b.bin",            "S:/Achi/BW/bee-b.bin" }},
		// { Achievement::Bee_crow,         { "S:/Achi/bee-crow.bin",         "S:/Achi/BW/bee-crow.bin" }},
		{ Achievement::Bee_g,            { "S:/Achi/bee-g.bin",            "S:/Achi/BW/bee-g.bin" }},
		{ Achievement::Bee_s,            { "S:/Achi/bee-s.bin",            "S:/Achi/BW/bee-s.bin" }},
		// { Achievement::Blocks_4lines,    { "S:/Achi/blocks-4lines.bin",    "S:/Achi/BW/blocks-4lines.bin" }},
		{ Achievement::Blocks_b,         { "S:/Achi/blocks-b.bin",         "S:/Achi/BW/blocks-b.bin" }},
		{ Achievement::Blocks_clear,     { "S:/Achi/blocks-clear.bin",     "S:/Achi/BW/blocks-clear.bin" }},
		{ Achievement::Blocks_g,         { "S:/Achi/blocks-g.bin",         "S:/Achi/BW/blocks-g.bin" }},
		{ Achievement::Blocks_s,         { "S:/Achi/blocks-s.bin",         "S:/Achi/BW/blocks-s.bin" }},
		{ Achievement::Bob_bombs,        { "S:/Achi/bob-bombs.bin",        "S:/Achi/BW/bob-bombs.bin" }},
		// { Achievement::Bob_green,        { "S:/Achi/bob-green.bin",        "S:/Achi/BW/bob-green.bin" }},
		// { Achievement::Bob_yellow,       { "S:/Achi/bob-yellow.bin",       "S:/Achi/BW/bob-yellow.bin" }},
		{ Achievement::Bonk_5,           { "S:/Achi/bonk-5.bin",           "S:/Achi/BW/bonk-5.bin" }},
		{ Achievement::Bonk_comeback,    { "S:/Achi/bonk-comeback.bin",    "S:/Achi/BW/bonk-comeback.bin" }},
		// { Achievement::Buttons_triple,   { "S:/Achi/buttons-triple.bin",   "S:/Achi/BW/buttons-triple.bin" }},
		{ Achievement::Buttons_win,      { "S:/Achi/buttons-win.bin",      "S:/Achi/BW/buttons-win.bin" }},
		{ Achievement::Capacitron_b,     { "S:/Achi/capacitron-b.bin",     "S:/Achi/BW/capacitron-b.bin" }},
		{ Achievement::Capacitron_fire,  { "S:/Achi/capacitron-fire.bin",  "S:/Achi/BW/capacitron-fire.bin" }},
		{ Achievement::Capacitron_g,     { "S:/Achi/capacitron-g.bin",     "S:/Achi/BW/capacitron-g.bin" }},
		{ Achievement::Capacitron_s,      { "S:/Achi/capacitron-s.bin",       "S:/Achi/BW/capacitron-s.bin" }},
		// { Achievement::Charlie_nomercy,   { "S:/Achi/charlie-nomercy.bin",    "S:/Achi/BW/charlie-nomercy.bin" }},
		// { Achievement::Charlie_yesmercy,  { "S:/Achi/charlie-yesmercy.bin",   "S:/Achi/BW/charlie-yesmercy.bin" }},
		// { Achievement::Dusty_shovel,      { "S:/Achi/dusty-shovel.bin",          "S:/Achi/BW/dusty-shovel.bin" }},
		{ Achievement::Dusty_rat,         { "S:/Achi/dusty-rat.bin",       "S:/Achi/BW/dusty-rat.bin" }},
		// { Achievement::Fred_copper,       { "S:/Achi/fred-copper.bin",        "S:/Achi/BW/fred-copper.bin" }},
		// { Achievement::Fred_detective,    { "S:/Achi/fred-detective.bin",     "S:/Achi/BW/fred-detective.bin" }},
		// { Achievement::Fred_veteran,      { "S:/Achi/fred-veteran.bin",       "S:/Achi/BW/fred-veteran.bin" }},
		{ Achievement::Hertz_3,           { "S:/Achi/hertz-3.bin",            "S:/Achi/BW/hertz-3.bin" }},
		{ Achievement::Marv_5,            { "S:/Achi/marv-5.bin",             "S:/Achi/BW/marv-5.bin" }},
		{ Achievement::Marv_life,         { "S:/Achi/marv-life.bin",          "S:/Achi/BW/marv-life.bin" }},
		// { Achievement::Marv_newspapers,   { "S:/Achi/marv-newspapers.bin",    "S:/Achi/BW/marv-newspapers.bin" }},
		{ Achievement::Planck_b,          { "S:/Achi/planck-b.bin",           "S:/Achi/BW/planck-b.bin" }},
		// { Achievement::Planck_chauffeur,  { "S:/Achi/planck-chauffeur.bin",   "S:/Achi/BW/planck-chauffeur.bin" }},
		{ Achievement::Planck_g,          { "S:/Achi/planck-g.bin",           "S:/Achi/BW/planck-g.bin" }},
		{ Achievement::Planck_juice,      { "S:/Achi/planck-juice.bin",       "S:/Achi/BW/planck-juice.bin" }},
		// { Achievement::Planck_nobrake,    { "S:/Achi/planck-nobrake.bin",     "S:/Achi/BW/planck-nobrake.bin" }},
		{ Achievement::Planck_s,          { "S:/Achi/planck-s.bin",           "S:/Achi/BW/planck-s.bin" }},
		{ Achievement::Resistron_b,       { "S:/Achi/resistron-b.bin",        "S:/Achi/BW/resistron-b.bin" }},
		{ Achievement::Resistron_deflect, { "S:/Achi/resistron-deflect.bin",  "S:/Achi/BW/resistron-deflect.bin" }},
		{ Achievement::Resistron_g,       { "S:/Achi/resistron-g.bin",        "S:/Achi/BW/resistron-g.bin" }},
		{ Achievement::Resistron_s,       { "S:/Achi/resistron-s.bin",        "S:/Achi/BW/resistron-s.bin" }},
		{ Achievement::Robby_asteroid,    { "S:/Achi/robby-asteroid.bin",     "S:/Achi/BW/robby-asteroid.bin" }},
		{ Achievement::Robby_sharp,       { "S:/Achi/robby-sharp.bin",        "S:/Achi/BW/robby-sharp.bin" }},
		{ Achievement::Snake_b,           { "S:/Achi/snake-b.bin",            "S:/Achi/BW/snake-b.bin" }},
		{ Achievement::Snake_fill,        { "S:/Achi/snake-fill.bin",         "S:/Achi/BW/snake-fill.bin" }},
		{ Achievement::Snake_g,           { "S:/Achi/snake-g.bin",            "S:/Achi/BW/snake-g.bin" }},
		{ Achievement::Snake_s,           { "S:/Achi/snake-s.bin",            "S:/Achi/BW/snake-s.bin" }},
		{ Achievement::Sparkly_nobrake,   { "S:/Achi/sparkly-nobrake.bin",    "S:/Achi/BW/sparkly-nobrake.bin" }},
		// { Achievement::Sparkly_overtake,  { "S:/Achi/sparkly-overtake.bin",   "S:/Achi/BW/sparkly-overtake.bin" }},
		{ Achievement::Sparkly_road,      { "S:/Achi/sparkly-road.bin",       "S:/Achi/BW/sparkly-road.bin" }},
		{ Achievement::Stacky_5,          { "S:/Achi/stacky-5.bin",           "S:/Achi/BW/stacky-5.bin" }},
		{ Achievement::Stacky_highrise,   { "S:/Achi/stacky-highrise.bin",    "S:/Achi/BW/stacky-highrise.bin" }},
		{ Achievement::Stacky_skyscraper, { "S:/Achi/stacky-skyscrapper.bin", "S:/Achi/BW/stacky-skyscrapper.bin" }},
		{ Achievement::Stacky_super,      { "S:/Achi/stacky-super.bin",       "S:/Achi/BW/stacky-super.bin" }},
};

const char* AchivementFile(Achievement achievement){
	const auto files = AchievementFiles.find(achievement);
	if(files == AchievementFiles.end()) return "";
	return files->second.unlocked;
}

const char* AchivementFileBW(Achievement achievement){
	const auto files = AchievementFiles.find(achievement);
	if(files == AchievementFiles.end()) return "";
	return files->second.locked;
}
