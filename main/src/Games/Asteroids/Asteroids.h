#ifndef BIT_FIRMWARE_ASTEROIDS_H
#define BIT_FIRMWARE_ASTEROIDS_H

#include "GameEngine/Game.h"
#include "Player.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"
#include "RoboCtrl/Robby.h"

namespace Asteroids {
class Asteroids : public Game {
public:
	Asteroids(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void onStop() override;
	void handleInput(const Input::Data& data) override;
	uint32_t getXP() const override;

private:
	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;
	int score = 0;
	static constexpr uint32_t MaxScore = 70;

	enum {
		Intro, Running, DeathAnim, DeathPause, Win
	} state = Intro;

	float introTimer = 0;
	static constexpr float introTime = 1.5f;
	float deathTimer = 0;
	static constexpr float deathPauseTime = 3.f;
	float winTimer = 0;
	static constexpr float winTime = 3.f;
	static constexpr float winAcceleration = 40.f;

	static constexpr glm::vec2 startPosition = { (128 - 36) / 2, (128 - 38) / 2 };

	uint8_t level = 0;
	static constexpr uint8_t MaxLevel = 4;
	void nextLevel();
	void gameOver();
	void spawnRandomAsteroid();


	//------------ Player ------------
	Player player;
	uint8_t life = 3;

	void playerHit();
	void updateInvincibility(float delta);
	bool invincible = false;
	float invincibilityTime = 0;
	float invincibilityBlinkDuration = 0.2f;
	std::shared_ptr<AnimRC> playerAnim;

	constexpr static float invincibilityDuration = 2.0f;
	constexpr static float PlayerRadius = 12;
	//------------ Player end ------------


	//------------ Bullet ------------
	GameObjPtr originalBullet;

	struct Bullet {
		GameObjPtr gObj;
		glm::vec2 velocity;

		bool operator==(const Bullet& other){
			return (velocity == other.velocity) && (gObj == other.gObj);
		}
	};

	constexpr static float bulletSpeed = 80.0f;

	std::vector<Bullet> bulletPool;
	void updateBullets(float deltaTime);
	void shootBullet();
	//------------ Bullet end ------------

	//------------ Asteroid ------------
	enum class AsteroidSize : uint8_t {
		Small, Medium, Large, Count
	};

	struct ImageDesc {
		const char* path;
		PixelDim dim;
	};
	static constexpr ImageDesc asteroidIcons[] = { { "/S1.raw", { 10, 10 } },
												   { "/M1.raw", { 27, 25 } },
												   { "/L1.raw", { 36, 37 } },
												   { "/S2.raw", { 12, 10 } },
												   { "/M2.raw", { 27, 25 } },
												   { "/L2.raw", { 36, 34 } } };

	struct Asteroid {
		GameObjPtr gObj;
		glm::vec2 velocity;
		AsteroidSize size;

		bool operator==(const Asteroid& other){
			return (velocity == other.velocity) && (gObj == other.gObj) && (size == other.size);
		}
	};

	constexpr static std::array<float, 3> asteroidSpeed = { 25.0f, 20.0f, 12.0f };
	constexpr static std::array<float, 3> asteroidRadius = { 5.0f, 11.0f, 17.5f };
	std::vector<Asteroid> asteroidPool;

	void asteroidHit(const Asteroid& asteroid);
	void createAsteroid(AsteroidSize size, glm::vec2 pos);
	void updateAsteroids(float deltaTime);

	struct {
		GameObject top;
		GameObject bot;
		GameObject left;
		GameObject right;
	} wrapWalls;
	constexpr static glm::vec2 wrapWallsSize = { 128.0f + 4 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 2,
												 128.0f + 4 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 2 };
	//------------ Asteroid end ------------

	bool leftHold = false;
	bool rightHold = false;

	std::shared_ptr<RoboCtrl::Robby> robot;
	static constexpr uint32_t MaxRobotPeriod = 1000;
	static constexpr uint32_t MinRobotPeriod = 10;

	static constexpr float MaxAsteroidDistance = 128 / 2;
	static constexpr float MinAsteroidDistance = PlayerRadius;
	static constexpr uint32_t WinLosePeriod = 200;
	void updateRobotPeriod();

	bool noMiss = true;
};

}

#endif //BIT_FIRMWARE_ASTEROIDS_H
