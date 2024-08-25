#ifndef BIT_FIRMWARE_ROBOTMANAGER_H
#define BIT_FIRMWARE_ROBOTMANAGER_H

#include "Services/Robots.h"
#include "Util/Threaded.h"
#include "Util/Events.h"
#include "Settings/Settings.h"
#include <misc/lv_color.h>
#include <unordered_set>

enum class Games : uint8_t {
	Blocks,
	Pong,
	Snake,
	WackyStacky,
	Artemis,
	MrBee,
	Bob,
	Buttons,
	Capacitron,
	Hertz,
	Marv,
	Resistron,
	Robby,
	Harald,
	Frank, // TODO this will be added later as a game, leave it in here but add it to games that are coming in the future
	Charlie,
	Fred,
	Planck,
	Dusty,
	Sparkly,
	COUNT
};

enum class Pet : uint8_t {
	Cow,
	Parrot,
	Frog,
	Dog,
	Sheep,
	Cat,
	COUNT
};

// Ordered by game (enum Games)
static const lv_color_t GameTextColors[] = {
		lv_color_make(255, 250, 250),
		lv_color_white(),
		lv_color_black(),
		lv_color_black(),
		lv_color_make(23, 21, 18),
		lv_color_white(),
		lv_color_make(255, 151, 0),
		lv_color_white(),
		lv_color_make(255, 174, 112),
		lv_color_make(255, 220, 140),
		lv_color_white(),
		lv_color_white(),
		lv_color_white(),
		lv_color_black(),
		lv_color_black(), // Frank, not used for now
		lv_color_black(),
		lv_color_black(),
		lv_color_black(),
		lv_color_white(),
		lv_color_white()
};

// Ordered by address (enum Robot (COUNT + enum Token if robot == COUNT))
static constexpr const char* RobotIcons[] = {
		"Bee",
		"Resis",
		"Arte",
		"Robby",
		"Marv",
		"Capa",
		"Bob",
		"Butt",
		"Hertz",
		"Harald",
		"Frank",
		"Charlie",
		"Fred",
		"Plank",
		"Dusty",
		"Sparkly",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
};

// Ordered by address (enum Robot (COUNT + enum Token if robot == COUNT))
static constexpr const char* RobotNames[] = {
		"Mr. Bee",
		"Resistron",
		"Artemis",
		"Robby",
		"Marv",
		"Capacitron",
		"Bob",
		"Buttons",
		"Hertz",
		"Harald",
		"Frank",
		"Charlie",
		"Fred",
		"Plank",
		"Dusty",
		"Sparkly",
		"Cow",
		"Parrot",
		"Frog",
		"Dog",
		"Sheep",
		"Mountain",
		"Cyber",
		"Fire",
		"Cat",
};

// Ordered by game (enum Game)
static constexpr const char* GameIcons[] = {
		"Blocks",
		"Pong",
		"Snake",
		"Stacky",
		"Arte",
		"Bee",
		"Bob",
		"Butt",
		"Capa",
		"Hertz",
		"Marv",
		"Resis",
		"Robby",
		"Harald",
		"Frank",
		"Charlie",
		"Fred",
		"Planck",
		"Dusty",
		"Sparkly"
		/*TODO add new game icons content*/
};

// Ordered by pet (enum Pet)
static constexpr const char* PetIcons[] = {
		"S:/Pets/Cow.bin",
		"S:/Pets/Parrot.bin",
		"S:/Pets/Frog.bin",
		"S:/Pets/Dog.bin",
		"S:/Pets/Sheep.bin",
		"S:/Pets/Cat.bin"
};

// Ordered by theme (enum Theme)
static constexpr const char* ThemeIcons[] = {
	"S:/Profile/theme-01-icon.bin",
	"S:/Profile/theme-02-icon.bin",
	"S:/Profile/theme-03-icon.bin",
	"S:/Profile/theme-04-icon.bin"
};

class RobotManager : private Threaded {
public:
	RobotManager();

	struct Event {
		enum { Inserted, Unknown, Remove } action;
		RobotData rob;
		bool isNew;
	};

	bool isUnlocked(Games game) const;
	bool isUnlocked(Theme theme) const;
	bool isUnlocked(Pet pet) const;
	bool isUnlocked(RobotData robot) const;

	static Games toGame(RobotData robot);
	static Theme toTheme(RobotData robot);
	static Pet toPet(RobotData robot);

	static const std::unordered_map<Games, RobotData> GameRobot;
	static const std::unordered_map<Theme, RobotData> ThemeRobot;
	static const std::unordered_map<Pet, RobotData> PetRobot;

private:
	std::array<bool, (uint8_t) Robot::COUNT + (uint8_t) Token::COUNT> unlocked = { false };

	void storeState();

	EventQueue events;
	void loop() override;

	static void sendEvent(Event evt, Facility type);

};


#endif //BIT_FIRMWARE_ROBOTMANAGER_H
