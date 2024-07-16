#ifndef BIT_FIRMWARE_ACHIEVEMENTTEXTDATA_HPP
#define BIT_FIRMWARE_ACHIEVEMENTTEXTDATA_HPP

struct AchievementText {
	const char* Title;
	const char* Description;
};

//TODO - adjust and add descriptions/titles
constexpr AchievementText AchievementTextData[(uint32_t) Achievement::COUNT] = {
		{ "Friendly fire",       "Shoot your own squid 100 times." },
		{ "Marksman",            "Win without shooting your own squid" },
		{ "Flapper 1",           "Score 50 points" },
		{ "Crow hopper",         "Fly over a crow" },
		{ "Flapper 3",           "Score 200 points" },
		{ "Flapper 2",           "Score 100 points" },
		{ "Blocks!",             "Clear 4 lines at once" },
		{ "Blokker 1",           "Score 500 points" },
		{ "Clean slate",         "Completely clear the field" },
		{ "Blokker 3",           "Score 2000 points" },
		{ "Blokker 2",           "Score 1000 points" },
		{ "No bombs",            "Win without eating any bombs" },
		{ "Green gourmet",       "Win while eating only green bulbs" },
		{ "Red gourmet",         "Win while eating only red bulbs" },
		{ "Undisputed",          "Win 5 games in a row" },
		{ "Comeback",            "Win after losing 0-2" },
		{ "Hips don't lie",      "Win while hitting every triple note" },
		{ "Moves like Jagger",   "Win without missing a single beat" },
		{ "Jumper 1",            "Score 250 points" },
		{ "Firebender",          "Eat a fireball after getting a potion" },
		{ "Jumper 3",            "Score 1000 points" },
		{ "Jumper 2",            "Score 500 points" },
		{ "No mercy",            "Win without letting a single trapped fly escape." },
		{ "Yes mercy",           "Lose every trapped fly" },
		{ "Expert excavator",    "Win without losing a life" },
		{ "",                    "Lose a battery to a rat" },
		{ "Copper",              "Catch all criminals with a single pair of handcuffs" },
		{ "Detective",           "Collect all tracks in a level without losing a life" },
		{ "Veteran",             "Catch 100 criminals" },
		{ "Optimal tuning",      "Win in three hits" },
		{ "",                    "Win 5 times in a row without losing any lives" },
		{ "",                    "Win once without losing any lives" },
		{ "Paperboy",            "Get hit by newspapers 100 times" },
		{ "Novice driver",       "Score 50 points" },
		{ "Chauffeur",           "Win without hitting a single obstacle" },
		{ "Expert driver",       "Score 200 points" },
		{ "More juice",          "Collect 10 batteries in a single run" },
		{ "Joyride",             "Complete a run without braking once" },
		{ "Intermediate driver", "Score 100 points" },
		{ "Space guardian 1",    "Score 50 points" },
		{ "Parry",               "Deflect an enemy bullet with your own" },
		{ "Space guardian 3",    "Score 200 points" },
		{ "Space guardian 2",    "Score 100 points" },
		{ "",                    "Win without colliding with an asteroid" },
		{ "Sharpshooter",        "Don't miss a single shot" },
		{ "Snaker 1",            "Score 10 points" },
		{ "Snake pro",           "Reach maximum snake length" },
		{ "Snaker 3",            "Score 100 points" },
		{ "Snaker 2",            "Score 50 points" },
		{ "",                    "Win without braking" },
		{ "",                    "Don't get overtaken a single time." },
		{ "",                    "Complete a run without straying off the road." },
		{ "Architect",           "Get 5 perfect drops in a row" },
		{ "Highrise",            "Reach a height of 50m" },
		{ "Skyscraper",          "Reach a height of 100m" },
		{ "Megastructure",       "Reach a height of 200m" },
};

#endif //BIT_FIRMWARE_ACHIEVEMENTTEXTDATA_HPP
