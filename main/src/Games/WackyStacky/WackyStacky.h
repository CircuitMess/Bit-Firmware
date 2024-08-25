#ifndef BIT_FIRMWARE_WACKYSTACKY_H
#define BIT_FIRMWARE_WACKYSTACKY_H

#include <cstdint>
#include <stack>
#include "GameEngine/Game.h"
#include "Games/Common/Score.h"
#include "Games/Common/Hearts.h"
#include "Util/Events.h"

namespace WackyStacky {
class WackyStacky : public Game {
public:
	explicit WackyStacky(Sprite& base);

protected:
	uint32_t getXP() const override;
	uint32_t getScore() const override;
	virtual void onLoad() override;
	virtual void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;

private:
	std::unique_ptr<Score> scoreDisplay;
	std::unique_ptr<Hearts> hearts;
	GameObjPtr hook;
	GameObjPtr floor;
	GameObjPtr hookedRobot;
	uint8_t lives = 3;
	uint8_t currentRobot = 0;
	uint16_t score = 0;
	float deadTimer = 0;
	int perfectCount = 0;

	float swingT = 0.0f;
	inline static constexpr const glm::vec2 SwingLimits = { -40.0f, 40.0f };
	inline static constexpr const float SwingSpeed = 1.5f;

	float dropT = 0.0f;
	bool dropping = false;
	struct {
		glm::vec2 pos;
		float rot;
	} dropStart;
	glm::vec2 dropSpeed;
	glm::vec2 dropPos;
	bool falling = false;
	float fallDir = 0;

	float towSwingT = 0.0f;
	inline static constexpr const float TowerSwingSpeed = 1.0f;

	bool scrolling = false;
	float scrollDelta = 0;
	static constexpr float TargetHeight = 95;
	inline static constexpr const uint8_t VisibleRobotCount = 3;
	struct Robot {
		GameObjPtr go;
		glm::vec2 pos = { 0, 0 };
		glm::vec2 posDiff = { 0, 0 };

		operator bool() const{ return go.operator bool(); }
	};
	std::array<Robot, VisibleRobotCount> visibleRobots;

	inline static constexpr const uint8_t CloudCount = 4;
	inline static constexpr const uint8_t ActiveCloudCount = 6;
	std::array<GameObjPtr, ActiveCloudCount> clouds;

	inline static constexpr PixelDim CloudDims[] {
			{ 39, 15 },
			{ 40, 22 },
			{ 24, 9 },
			{ 40, 10 }
	};

	inline static constexpr const char* CloudPaths[] = {
		"/Cloud1.raw",
		"/Cloud2.raw",
		"/Cloud3.raw",
		"/Cloud4.raw",
	};

	inline static constexpr const char* RobotPaths[] = {
		"/robot1.raw",
		"/robot2.raw",
		"/robot3.raw",
		"/robot4.raw",
		"/robot5.raw",
		"/robot6.raw",
		"/robot7.raw",
	};

private:
	void swingAnim(float dt);
	void applyHookRot(float deg);

	void towerSwingAnim(float dt);

	void drop();
	void dropAnim(float dt);
	void dropped();

	bool scrollStart();
	void scrollAnim(float dt);

	void spawnRobot();
	void updateRobotPos();
	void miss();
	void robotFallen();

	inline static constexpr std::string getRobotPath(uint8_t robot) {
		if(robot > 6){
			return "";
		}

		return "/robot" + std::to_string(robot + 1) + ".raw";
	}

	inline static constexpr PixelDim getRobotDim(uint8_t robot) {
		switch(robot){
			case 0:{
				return { 39, 23 };
			}
			case 1:{
				return { 51, 26 };
			}
			case 2:{
				return { 31, 24 };
			}
			case 3:{
				return { 35, 26 };
			}
			case 4:{
				return { 31, 20 };
			}
			case 5:{
				return { 25, 26 };
			}
			case 6:{
				return { 39, 30 };
			}
			default:{
				break;
			}
		}

		return { 0, 0 };
	}
};
}

#endif //BIT_FIRMWARE_WACKYSTACKY_H