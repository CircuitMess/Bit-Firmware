#ifndef BIT_FIRMWARE_PLANCK_H
#define BIT_FIRMWARE_PLANCK_H

#include "GameEngine/Game.h"
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"

namespace Planck {
class Planck : public Game {
public:
	explicit Planck(Sprite& canvas);

protected:
	void onLoad() override;
	void onStart() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	inline uint32_t getXP() const override { return score * 5; }
	inline uint32_t getScore() const override { return score; }

private:
	inline static constexpr const uint8_t VerticalTiles = 5;
	inline static constexpr const uint8_t HorizontalTiles = 3;
	inline static constexpr const float HorizontalSpeed = 80.0f;
	inline static constexpr const float StraightlineSpeedLimit = 150;
	inline static constexpr const uint32_t BoostDuration = 1.0f;
	inline static constexpr const float BoostAccelerationRate = 75;
	inline static constexpr const uint32_t AirDuration = 1500;
	inline static constexpr const float PassiveDeceleration = -9.0f;
	inline static constexpr const float GasBrakeAcceleration = 100.0f;
	inline static constexpr const float BatteryDischargeRate = 0.25f;

	inline static constexpr const float JumpMaxZoom = 1.3f;

	inline static constexpr const float MinimumRampSpeed = 50; //speed below this will not result in a ramp jump

	inline static constexpr const uint16_t CarYPos = 128 - 30 - 5;

	bool invincible = false;
	float invincibilityTime = 0;
	static constexpr float InvincibilityBlinkDuration = 0.25f;
	static constexpr float InvincibilityDuration = 1.5f;

	uint8_t nextObstacleDistance = 4;
	inline static constexpr const uint8_t MinNextObstacleDistance = 2;
	inline static constexpr const uint8_t MaxNextObstacleDistance = 3;

	inline static constexpr const char* Obstacles[] = {
			"/cones.raw",
			"/rail.raw",
			"/trash.raw",
			"/ramp.raw",
			"/boost.raw",
			"/hole.raw",
	};

	inline static constexpr const char* Pickups[] = {
			"/bat.raw",
			"/pickup1.raw",
			"/pickup2.raw",
	};

	uint32_t score = 0;
	uint32_t lives = 3;
	float battery = 1.0f;
	GameObjPtr car;
	std::shared_ptr<SpriteRC> carRC;
	GameObjPtr batteryBar;
	GameObjPtr batteryLine;
	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;
	std::array<GameObjPtr, 14> leftEdge;
	std::array<GameObjPtr, 14> rightEdge;
	std::array<GameObjPtr, VerticalTiles * HorizontalTiles> road;
	uint8_t sinceGenerated = 0;
	std::array<bool, HorizontalTiles> rampsGenerated = { false };
	std::array<bool, HorizontalTiles> holesGenerated = { false };
	uint8_t rowToGenerate = 0;
	float direction = 0.0f;
	float poweredAcceleration = 0.0f;
	float boostAcceleration = 0.0f;
	float speed = 0;
	glm::vec2 speedLimits = { 0, StraightlineSpeedLimit };
	float boostProgress = 0;
	bool boosting = false;
	uint32_t lastAir = 0;
	std::set<GameObjPtr> pickups;

	enum TileType {
		Cones,
		Rail,
		Trash,
		Ramp,
		Boost,
		Hole,
		Road
	};

	//hole needs to be generated after each ramp
	bool holePending[HorizontalTiles] = { 0 };

private:
	void generateRoad();
	bool onCollision();
	void onBoost();
	void onRamp();
	void onHealthUp();
	void onBatteryUp();
	void onPickup();
	GameObjPtr createTile(TileType type);
	void setBattery(float percent);
};
} // Planck

#endif //BIT_FIRMWARE_PLANCK_H