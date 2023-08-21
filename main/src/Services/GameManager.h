#ifndef BIT_FIRMWARE_GAMEMANAGER_H
#define BIT_FIRMWARE_GAMEMANAGER_H

#include "Services/Robots.h"
#include "Util/Threaded.h"
#include "Util/Events.h"
#include <unordered_set>

enum class Games : uint8_t { Blocks, Pong, Snake, Artemis, MrBee, Bob, Buttons, Capacitron, Hertz, Marv, Resistron, Robby, COUNT };

// Ordered by address (enum Robot)
static constexpr const char* RobotIcons[] = {
		"Bee",
		"Resis",
		"Arte",
		"Robby",
		"Marv",
		"Capa",
		"Bob",
		"Butt",
		"Hertz"
};

// Ordered by address (enum Robot)
static constexpr const char* RobotNames[] = {
		"Mr. Bee",
		"Resistron",
		"Artemis",
		"Robby",
		"Marv",
		"Capacitron",
		"Bob",
		"Buttons",
		"Hertz"
};

// Ordered by game (enum Game)
static constexpr const char* GameIcons[] = {
		"Blocks",
		"Bonk",
		"Snake",
		"Arte",
		"Bee",
		"Bob",
		"Butt",
		"Capa",
		"Hertz",
		"Marv",
		"Resis",
		"Robby"
};

class GameManager : private Threaded {
public:
	GameManager();

	struct Event {
		enum { Inserted, Unknown } action;
		Robot rob;
		bool isNew;
	};

	bool isUnlocked(Games game);

	static const std::unordered_map<Games, Robot> GameRobot;

private:
	std::unordered_set<Robot> unlocked;

	void storeState();

	EventQueue events;
	void loop() override;

};


#endif //BIT_FIRMWARE_GAMEMANAGER_H
