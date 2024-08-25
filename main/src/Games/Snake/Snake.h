#ifndef BIT_FIRMWARE_SNAKE_H
#define BIT_FIRMWARE_SNAKE_H

#include "GameEngine/Game.h"
#include "Games/Common/Score.h"
#include "GameEngine/Rendering/TextRC.h"

class Snake : public Game {
public:
	Snake(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	uint32_t getXP() const override;
	inline uint32_t getScore() const override { return score; }

private:
	enum class State {
		Running, GameOver, GameWin, DonePause
	} state = State::Running;

	GameObjPtr bg;
	std::vector<GameObjPtr> snake;
	std::shared_ptr<Sprite> headSprite;
	GameObjPtr food;
	std::unique_ptr<Score> scoreElement;

	uint32_t score = 0;
	glm::vec2 speed = { 0, 0 };
	float baseSpeed = StartingSpeed;
	float moveBuffer = 0; //this accumulates to TileDim over time, until the position changes
	bool inputInterrupt = true; //for handling input cases inside one tile movement timeframe
	float gameWinCounter = 0;
	static constexpr uint32_t XPCutoff = 100;

	struct FoodDesc {
		const char* path;
		PixelDim dim;
	};
	static constexpr FoodDesc Foods[] = {
			{ "/Apple.raw",     { 5,  5 } },
			{ "/Banana.raw",    { 10, 11 } },
			{ "/Blueberry.raw", { 5,  5 } },
			{ "/Cherry.raw",    { 6,  7 } },
			{ "/Grapes.raw",    { 5,  9 } },
			{ "/Icecream.raw",  { 5,  11 } },
			{ "/Lemon.raw",     { 9,  7 } },
			{ "/Plum.raw",      { 7,  4 } },
			{ "/Sausage.raw",   { 7,  9 } },
	};


	//26x26 grid with 5x5 tiles, 4 pixels padding on edges
	static constexpr PixelDim TileDim = { 5, 5 };
	static constexpr int8_t PaddingGap = -1;
	static constexpr PixelDim GridDim = { 26, 26 };
	float donePause = 0;

	static constexpr PixelDim HitboxDim = { 3, 3 };
	static constexpr uint8_t StartingLength = 2;
	static constexpr float StartingSpeed = 50;
	static constexpr float SpeedIncrement = 1.0 / 24.0;
	static constexpr uint8_t GameWinPause = 2;
	void drawHead(Sprite& head);
	void addSegment();
	void moveSnake(glm::vec2 move);
	void gameOver();
	void foodEaten(bool initial = false);

	void checkAchi();

};


#endif //BIT_FIRMWARE_SNAKE_H
