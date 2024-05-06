#ifndef BIT_FIRMWARE_MARVGAME_H
#define BIT_FIRMWARE_MARVGAME_H

#include "GameEngine/Game.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Player.h"
#include "../Common/Hearts.h"
#include "GameEngine/Rendering/TextRC.h"
#include "TileManager.h"
#include "RoboCtrl/Marv.h"

namespace MarvGame {
class MarvGame : public Game {
	friend Player;
public:
	MarvGame(Sprite& canvas);
	float getSpeed();

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;
	uint32_t getXP() override;

private:
	std::unique_ptr<Player> duck;
	std::unique_ptr<Hearts> hearts;

	struct Obstacle {
		File file;
		PixelDim dim;
		std::vector<glm::vec2> points;
	};
	void setupObstacles();
	void spawn();
	void scoreUp();
	void duckHit();

	const int tileDim = 16;
	const int topY = 128 - 18;
	const int tilesPerArray = 5;
	float spawnValue = 0.0f;
	float spawnRate = 2.0f;
	const float spawnIncrement = 0.02f;
	const int scoreMax = 40;
	float speed = 75.0f;
	static constexpr float TilesSpeedMultiplier = 0.45;
	const float speedMax = 120.0f;
	const float speedIncrement = (speedMax - speed) / scoreMax;
	bool isDone = false;
	int score = 0;
	uint8_t life = 3;

	enum EndState {
		Running, Win, Lose
	} endPauseState = Running;
	float endPauseCounter = 0;
	static constexpr float DeathPause = 2.5;
	static constexpr float WinPause = 2.6;

	std::unique_ptr<TileManager> tileManager;
	std::shared_ptr<TextRC> scoreTextRC;
	GameObjPtr leftWall;
	GameObjPtr leftWallObject;
	GameObjPtr bg;
	GameObjPtr podium;
	std::vector<Obstacle> obstacleUnder;
	std::vector<Obstacle> obstacleOver;
	std::set<uint8_t> obstacleIndex;
	std::vector<GameObjPtr> movingObjects;
	std::vector<GameObjPtr> movingTiles;

	std::shared_ptr<RoboCtrl::Marv> robot;
	static constexpr uint32_t StartingLightsSpeed = 500;
};
}

#endif //BIT_FIRMWARE_MARVGAME_H
