#ifndef BIT_FIRMWARE_WACKYSTACKY_H
#define BIT_FIRMWARE_WACKYSTACKY_H

#include <cstdint>
#include "GameEngine/Game.h"
#include "Games/Common/Score.h"

namespace WackyStacky {
	class WackyStacky : public Game {
	public:
		explicit WackyStacky(Sprite& base);

	protected:
		uint32_t getXP() const override;
		virtual void onLoad() override;
		virtual void onLoop(float deltaTime) override;

	private:
		std::unique_ptr<Score> scoreDisplay;
		GameObjPtr hook;
		float swingDir = 1.0f;

		inline static constexpr const glm::vec2 SwingLimits = { -40.0f, 40.0f};

		inline static constexpr const float SwingSpeed = 35.0f;

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

		inline static constexpr const uint32_t HookSwingTime = 1000;

	private:
		void rotateHook(float deg) const;
	};
}

#endif //BIT_FIRMWARE_WACKYSTACKY_H