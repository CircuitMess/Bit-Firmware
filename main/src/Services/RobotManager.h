#ifndef BIT_FIRMWARE_ROBOTMANAGER_H
#define BIT_FIRMWARE_ROBOTMANAGER_H

#include "Services/Robots.h"
#include "Util/Threaded.h"
#include "Util/Events.h"
#include "Settings/Settings.h"
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
	Frank,
	RoboSpider, // TODO name
	Fred,
	Plank,
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
	Cat
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
		"RoboSpider", // TODO name
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
		"RoboSpider", // TODO name
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

// Ordered by game (enum Game)
static constexpr const char* GameIcons[] = {
		"Blocks",
		"Bonk",
		"Snake",
		"WackyStacky",
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
		"RoboSpider", // TODO name
		"Fred",
		"Plank",
		"Dusty",
		"Sparkly"
		/*TODO add new game icons content*/
};

class RobotManager : private Threaded {
public:
	RobotManager();

	struct Event {
		enum { Inserted, Unknown, Remove } action;
		RobotData rob;
		bool isNew;
	};

	bool isUnlocked(Games game);
	bool isUnlocked(Theme theme);
	bool isUnlocked(Pet pet);

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
