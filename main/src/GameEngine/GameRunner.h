#ifndef BIT_FIRMWARE_GAMERUNNER_H
#define BIT_FIRMWARE_GAMERUNNER_H


#include "Util/Threaded.h"
#include "Game.h"
#include "Services/RobotManager.h"
#include "Services/Allocator.h"

class GameRunner{
public:
	GameRunner(Display& display, Allocator* alloc = nullptr);

	Games getCurrent();

	void startGame(Games game);
	void endGame();
	void exitGame();

	void loop();
	void resume();

private:
	Display& display;
	Allocator* alloc = nullptr;

	uint64_t lastMicros;

	std::unique_ptr<Game> currentGame;
	Games currentGameEnum = Games::COUNT;

	static constexpr uint32_t FrameTime = 25; // [ms]

};


#endif //BIT_FIRMWARE_GAMERUNNER_H
