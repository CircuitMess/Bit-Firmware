#ifndef BIT_FIRMWARE_GAMEMANAGER_H
#define BIT_FIRMWARE_GAMEMANAGER_H

#include "Services/Robots.h"
#include "Util/Threaded.h"
#include "Util/Events.h"
#include <unordered_set>

enum class Games : uint8_t { Blocks, Pong, Snake, Artemis, MrBee, Bob, Buttons, Capacitron, Hertz, Marv, Resistron, Robby, COUNT };

class GameManager : private Threaded {
public:
	GameManager();

	struct Event {
		enum { Inserted } action;
		Robot rob;
		bool isNew;
	};

	bool isUnlocked(Games game);

private:
	std::unordered_set<Robot> unlocked;

	void storeState();

	EventQueue events;
	void loop() override;

};


#endif //BIT_FIRMWARE_GAMEMANAGER_H
