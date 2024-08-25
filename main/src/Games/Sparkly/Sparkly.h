#ifndef BIT_FIRMWARE_SPARKLY_H
#define BIT_FIRMWARE_SPARKLY_H

#include "GameEngine/Game.h"
#include "RaceTime.h"
#include "Util/stdafx.h"

namespace Sparkly {
class Sparkly : public Game {
public:
	explicit Sparkly(Sprite& canvas);

	inline virtual uint32_t getXP() const override {
		return getScore();
	}

	inline virtual uint32_t getScore() const override {
		if(finishTime <= 0.0f){
			return 0;
		}

		static constexpr float AvgTime = 45.0f;
		static constexpr float AvgScore = 100;

		int score = AvgScore;

		if(finishTime >= AvgTime){
			const float diff = std::min(AvgTime - 5, finishTime - AvgTime);
			score -= (int) std::round(AvgScore * diff / AvgTime);
		}else{
			const float diff = std::min(AvgTime / 2.0f, AvgTime - finishTime);
			score += (int) std::round(AvgScore * diff / (AvgTime / 2.0f));
		}

		return score;
	}

protected:
	virtual void onLoad() override;
	virtual void onStop() override;
	virtual void onLoop(float deltaTime) override;
	virtual void preRender(Sprite& canvas) override;
	virtual void handleInput(const Input::Data& data) override;

private:;
	glm::vec3 forward = { 1.0f, 0.0f, 0.0f };
	glm::vec3 camPos = { -8.0f, 1.2f, 0.5f };
	glm::mat4 vpMat; // view-projection matrix
	glm::mat4 vpInv; // inverted view-projection matrix

	float acceleration = 0.0f;
	float spd = 0.0f; // Forward/backward
	float spdZ = 0.0f; // Left/right
	float rotZ = -90.0f;

	File skybox;

	static constexpr const float Friction = 0.2f;

	static constexpr const float CameraAngle = 15.0f;

	static constexpr DRAM_ATTR glm::vec3 Up = { 0, 0, 1 };

	static constexpr DRAM_ATTR int Field[20][20] = {
			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 },
			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 34, 23, 23, 23, 23, 23, 23, 32, 30, 30 },
			{ 30, 30, 30, 30, 30, 30, 30, 30, 30, 34, 21, 4, 7, 7, 7, 7, 8, 25, 30, 30 },
			{ 30, 31, 23, 23, 23, 37, 30, 30, 34, 21, 11, 19, 26, 26, 26, 20, 9, 25, 30, 30 },
			{ 30, 24, 6, 7, 15, 22, 37, 30, 38, 20, 16, 22, 37, 30, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 19, 20, 16, 22, 37, 30, 38, 20, 16, 22, 37, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 38, 20, 16, 22, 37, 30, 38, 20, 0, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 38, 20, 16, 22, 37, 30, 24, 9, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 38, 20, 16, 22, 37, 24, 9, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 28, 17, 29, 30, 30, 30, 38, 20, 16, 22, 21, 9, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 38, 20, 2, 7, 14, 25, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 38, 26, 26, 26, 27, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 30, 34, 23, 23, 23, 32, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 30, 34, 21, 4, 7, 8, 25, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 30, 34, 21, 11, 19, 20, 9, 25, 24, 9, 25, 30, 30 },
			{ 30, 24, 9, 25, 30, 30, 30, 34, 21, 11, 19, 35, 24, 9, 22, 21, 9, 25, 30, 30 },
			{ 30, 24, 9, 22, 23, 23, 23, 21, 11, 19, 35, 30, 24, 12, 7, 7, 14, 25, 30, 30 },
			{ 30, 24, 12, 7, 7, 7, 7, 13, 19, 35, 30, 30, 36, 26, 26, 26, 26, 27, 30, 30 },
			{ 30, 36, 26, 26, 26, 26, 26, 26, 35, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30 },
	};

	static constexpr const char* DRAM_ATTR BillboardAssets[] = {
			"/Bush1.raw",
			"/Bush2.raw",
			"/Poster1.raw",
			"/Poster2.raw",
			"/Tree1.raw",
			"/Tree2.raw",
	};

	static constexpr PixelDim DRAM_ATTR BillboardAssetDims[] = {
			{ 32, 28 },
			{ 24, 21 },
			{ 66, 65 },
			{ 66, 65 },
			{ 45, 47 },
			{ 31, 52 },
	};

	struct BillboardInfo{
		glm::vec2 position;
		size_t assetId;
	};

	static constexpr BillboardInfo DRAM_ATTR Billboards[] = {
			// TODO myb add in the future
	};

	struct EnemyMovementPoint{
		float timestamp = 0.0f;
		glm::vec2 pos = glm::vec2{ 0.0f };
		float rotation = 0.0f;
	};

	static constexpr const EnemyMovementPoint EnemyMovementPoints[] = {
			{.timestamp = 0.0f, .pos = { -7.5f, 1.0f }, .rotation = -90.0f},
			{.timestamp = 6.0f, .pos = { -7.0f, -5.15f }, .rotation = -90.0f},
			{.timestamp = 8.0f, .pos = { -4.5f, -4.25f }, .rotation = 35.0f},
			{.timestamp = 11.0f, .pos = { -1.2f, -0.66f }, .rotation = 47.0f},
			{.timestamp = 15.0f, .pos = { 1.25f, -0.75f }, .rotation = -27.0f},
			{.timestamp = 18.0f, .pos = { 0.5f, -4.0f }, .rotation = -125.0f},
			{.timestamp = 22.0f, .pos = { 3.5f, -7.3f }, .rotation = -0.0f},
			{.timestamp = 27.0f, .pos = { 6.5f, -2.5f }, .rotation = 90.0f},
			{.timestamp = 31.0f, .pos = { 6.5f, 4.7f }, .rotation = 90.0f},
			{.timestamp = 35.0f, .pos = { 5.5f, 7.0f }, .rotation = 175.0f},
			{.timestamp = 38.0f, .pos = { 3.0f, 6.0f }, .rotation = 220.0f},
			{.timestamp = 44.0f, .pos = { -0.5f, 6.3f }, .rotation = 150.0f},
			{.timestamp = 50.0f, .pos = { -4.8f, 8.3f }, .rotation = 180.0f},
			{.timestamp = 58.0f, .pos = { -8.0f, -2.0f }, .rotation = 280.0f},
	};

	const glm::mat4 Proj;

	std::vector<GameObjPtr> billboardGameObjs;

	GameObjPtr playerCar;
	GameObjPtr enemyCar;
	GameObjPtr startCountdown;
	GameObjPtr speedLine;
	std::unique_ptr<RaceTime> raceTimeElement;

	glm::vec3 enemyPos = { -7.5f, 0.5f, 0.0f };
	glm::vec3 enemyForward = { 1.0f, 0.0f, 0.0f };
	float enemyAngle = -90;
	size_t enemyTargetStep = 1;

	static constexpr const glm::vec<2, int> CarTopLeft = { 50, 79 };
	static constexpr const glm::vec<2, int> CarTopRight = { 79, 79 };
	static constexpr const glm::vec<2, int> CarBttmLeft = { 43, 93 };
	static constexpr const glm::vec<2, int> CarBttmRight = { 86, 93 };

	struct CollisionInfo {
		int spriteIndex = -1;
		glm::vec<2, int> spriteCoords = { 0, 0 };
		bool skybox = false;
	};

	CollisionInfo TopLeftCollision;
	CollisionInfo TopRightCollision;
	CollisionInfo BttmLeftCollision;
	CollisionInfo BttmRightCollision;

	bool inputEnabled = false;
	float timeSinceGameStart = 0.0f;
	float timeSinceRaceStart = 0.0f;
	float finishTime = 0.0f;
	int finishCrossings = 0;
	bool inFinish = false;

	Input::Data::Action lastA = Input::Data::Release;
	Input::Data::Action lastB = Input::Data::Release;

	bool lPressed = false;
	bool rPressed = false;

	bool braked = false;
	bool hitBarrier = false;

private:
	void sampleGround(Sprite& canvas);
	uint16_t sampleSkybox(int x, int y);
	void movement(float dt);
	void positionBillboards();
	static bool isCollisionBlocking(const CollisionInfo& collision);
	CollisionInfo getCollision(int x, int y) const;
	uint16_t isColliding() const;
	uint16_t isCollidingTemporary() const;
};
} // Sparkly

#endif //BIT_FIRMWARE_SPARKLY_H