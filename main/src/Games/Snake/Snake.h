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

private:
	enum class State {
		Running, GameOver
	} state = State::Running;

	GameObjPtr bg;
	std::vector<GameObjPtr> snake;
	std::shared_ptr<Sprite> headSprite;
	GameObjPtr food;
	std::unique_ptr<Score> scoreElement;

	uint32_t score = 0;
	glm::vec2 speed = { 0, 0 };
	float moveBuffer = 0; //this accumulates to TileDim over time, until the position changes
	bool inputInterrupt = true; //for handling input cases inside one tile movement timeframe

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
			{ "/Pear.raw",      { 5,  8 } },
			{ "/Plum.raw",      { 7,  4 } },
			{ "/Sausage.raw",   { 7,  9 } },
	};


	//24x24 grid with 5x5 tiles, 4 pixels padding on edges
	static constexpr PixelDim TileDim = { 5, 5 };
	static constexpr uint8_t PaddingGap = 4;
	static constexpr PixelDim GridDim = { 24, 24 };

	static constexpr PixelDim HitboxDim = { 3, 3 };
	static constexpr uint8_t StartingLength = 2;
	static constexpr float BaseSpeed = 50; //TODO - speed up as game progresses

	void drawHead(Sprite& head);
	void addSegment();
	void moveSnake(glm::vec2 move);
	void gameOver();
	void foodEaten(bool initial = false);
};


#endif //BIT_FIRMWARE_SNAKE_H
