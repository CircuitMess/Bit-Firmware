#ifndef BIT_FIRMWARE_FLAPPY_H
#define BIT_FIRMWARE_FLAPPY_H

#include "GameEngine/Game.h"
#include "GameEngine/Rendering/AnimRC.h"
#include <deque>
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"
#include "RoboCtrl/MrBee.h"

class Flappy : public Game {
public:
	Flappy(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void onStart() override;
	void onStop() override;
	uint32_t getXP() const override;
	inline uint32_t getScore() const override { return score; }

private:
	static constexpr float gravity = 150.0f;
	float speedX = 60.0f;
	static constexpr float flapSpeedY = 80.0f;
	static constexpr float duckPosX = 25;
	static constexpr float obstacleSpacing = 75;
	static constexpr uint8_t MinimumGap = 50;

	static constexpr uint8_t ScoreCutoff = 60;

	enum {
		FlyIn, Wait, Play, FallOut
	} state = FlyIn;

	float entry = 0;
	float entryDuration = 1.0f;

	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;

	uint8_t life = 3;
	void die();

	GameObjPtr bee;
	std::shared_ptr<AnimRC> anim;
	glm::vec2 velocity = { speedX, 0 };

	int score = 0;

	struct ObstaclePair {
		GameObjPtr top;
		GameObjPtr bot;
		bool passed;
	};
	std::deque<ObstaclePair> obstacles;

	void updateDuck(float delta);
	void updateObstacles(float delta);

	void createObstaclePair();
	void resetDuck();

	void handleInput(const Input::Data& data) override;

	struct ObstacleDesc {
		const char* path;
		PixelDim dim;
		std::initializer_list<glm::vec2> collision;
	};

	static constexpr ObstacleDesc TopObstacles[] = {
			{ "/up1.raw", { 15, 100 }, { { 6,  99 }, { 0, 93 }, { 5,  0 }, { 8,  0 },  { 12, 78 } } },
			{ "/up2.raw", { 49, 53 },  { { 19, 51 }, { 0, 30 }, { 2,  0 }, { 48, 0 } } },
			{ "/up3.raw", { 23, 64 },  { { 0, 0 }, { 10, 63 }, { 12, 63 }, { 22, 0 } } }, // tall
			{ "/up4.raw", { 22, 49 },  { { 0, 0 }, { 12, 44 }, { 21, 9 }, { 21, 0 } } }, // low
			{ "/up5.gif", { 37, 25 },  { { 26, 24 }, { 0, 11 }, { 1,  1 }, { 36, 8 } } }
	};

	static constexpr ObstacleDesc BotObstacles[] = {
			{ "/down1.raw", { 23, 73 }, { { 0,  72 }, { 4, 0 },  { 16, 0 }, { 21, 72 } } },
			{ "/down2.raw", { 22, 49 }, { { 0,  48 }, { 1, 0 },  { 19, 0 }, { 21, 48 } } },
			{ "/down3.raw", { 18, 46 }, { { 8,  45 }, { 0, 30 }, { 1,  3 }, { 8,  0 }, { 17, 4 },  { 17, 27 } } },
			{ "/down4.raw", { 27, 68 }, { { 13, 67 }, { 0, 56 }, { 5,  4 }, { 12, 0 }, { 21, 4 },  { 26, 50 } } },
			{ "/down5.raw", { 36, 51 }, { { 0,  50 }, { 0, 20 }, { 11, 0 }, { 29, 2 }, { 35, 41 }, { 35, 50 } } },
			{ "/down6.raw", { 19, 80 }, { { 9,  79 }, { 0, 22 }, { 0,  0 }, { 18, 0 }, { 18, 22 } } }
	};

	std::shared_ptr<RoboCtrl::MrBee> robot;
};


#endif //BIT_FIRMWARE_FLAPPY_H
