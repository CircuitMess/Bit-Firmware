#ifndef BIT_FIRMWARE_CAPACITRONGAME_H
#define BIT_FIRMWARE_CAPACITRONGAME_H

#include "GameEngine/Game.h"
#include "TileManager.h"
#include "Player.h"
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"
#include "RoboCtrl/Capacitron.h"

namespace CapacitronGame {

class TileManager;

class CapacitronGame : public Game {
	friend TileManager;
	friend Player;
public:
	CapacitronGame(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;
	uint32_t getXP() override;

private:
	std::unique_ptr<TileManager> tileManager;
	std::vector<GameObjPtr> tileObjs;
	std::vector<std::set<GameObjPtr>> padObjs; //pads are split in a queue of "levels", which are sets of 1 or more pad
	GameObjPtr bottomTileWall;
	GameObjPtr playerObj;
	GameObjPtr playerLegsObj;
	std::unique_ptr<Player> player;
	std::vector<GameObjPtr> powerupObjs;

	GameObjPtr originalFireball; //out-of-screen object used for its render component
	std::vector<GameObjPtr> fireballObjs;
	std::shared_ptr<AnimRC> fireballAnimRC; //render component passed to fireball instances


	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;

	static constexpr float speed = 5;
	static constexpr float JumpY = 42; //distance covered by one jump
	static constexpr float Gravity = 150.0f;
	static constexpr float JumpYExtra = 20; //extra leeway so that the player jumps up and over the next platform
	static constexpr float JumpSpeed = -Gravity + JumpY - JumpYExtra;
	static constexpr float TrampolineSpeed = 1.3 * JumpSpeed;
	static constexpr PixelDim PlayerSize = { 20, 30 };
	static constexpr uint8_t PlayerLegsHitboxWidth = 12;
	static constexpr float FireballSpeed = 25;
	static constexpr float MinimumSurface = 0.2;
	static constexpr float StartingSurface = 0.6;
	static constexpr float MaxDifficultyScore = 60; //score after which difficulty doesn't increase anymore
	static constexpr float FireballStartingInterval = 5;
	static constexpr float FireballMinimumInterval = 2;
	static constexpr uint32_t PowerupsStartScore = 15; //score after which powerups will be spawned
	static constexpr uint8_t PowerupsStartingRate = 25;
	static constexpr uint8_t PowerupsMinimumRate = 2;

	uint32_t score = 0;
	int8_t lives = 3;
	bool halfHeartCollected = false;

	void createPad(float surface, bool powerupsEnabled, uint8_t powerupRate);
	float counter = 0;
	bool cameraShifting = false;
	float camShiftDistance = 0;
	float totalShift = 0;
	float fireballTimer = 0;
	float fireballInterval = FireballStartingInterval;

	void powerupSpawned(Powerup powerup);
	void spawnFireball();
	void cleanupPads();
	virtual void exit() override;

	std::shared_ptr<RoboCtrl::Capacitron> robot;
};

}

#endif //BIT_FIRMWARE_CAPACITRONGAME_H
