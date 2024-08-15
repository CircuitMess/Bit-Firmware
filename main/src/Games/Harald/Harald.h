#ifndef BIT_FIRMWARE_HARALD_H
#define BIT_FIRMWARE_HARALD_H

#include "GameEngine/Game.h"
#include "Score.h"

namespace Harald {
class Harald : public Game {
public:
	explicit Harald(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;

	inline uint32_t getXP() const override { return score / 12; };
	inline uint32_t getScore() const override { return score; }

private:

	enum class State : uint8_t {
		Game, ExitAnim
	} state = State::Game;

	void gameLose();
	void gameWin();
	void startExitAnim();

	static constexpr int32_t ExitAnimYPush = 50;
	static constexpr float gravity = 125.0f;
	static constexpr uint32_t MaxSpeedX = 50;
	static constexpr uint32_t MinSpeedX = 20;
	static constexpr uint32_t ExitAnimPause = 500;
	uint32_t exitAnimStartTime = 0;


	struct GameElement {
		uint8_t id = 0; // 0 - 10
		float speedX = 0;
		float speedY = 0;
		GameObjPtr gameObj;
	};

	std::array<std::array<GameElement, 4>, 4> elements;
	uint32_t score = 0;
	std::unique_ptr<Score> scoreElement;
	std::unique_ptr<Score> bestScoreElement;

	inline static constexpr const char* Icons[] = {
			"/Tile01.raw",
			"/Tile02.raw",
			"/Tile03.raw",
			"/Tile04.raw",
			"/Tile05.raw",
			"/Tile06.raw",
			"/Tile07.raw",
			"/Tile08.raw",
			"/Tile09.raw",
			"/Tile10.raw",
			"/Tile11.raw",
	};
};
} // Harald

#endif //BIT_FIRMWARE_HARALD_H