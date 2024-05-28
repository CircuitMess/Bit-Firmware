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
        virtual ~WackyStacky();

	protected:
		uint32_t getXP() const override;
		virtual void onLoad() override;
		virtual void onLoop(float deltaTime) override;

	private:
		std::unique_ptr<Score> scoreDisplay;
        std::unique_ptr<Hearts> hearts;
        GameObjPtr hook;
		GameObjPtr floor;
        GameObjPtr hookedRobot;
        EventQueue queue;
		float swingDir = 1.0f;
        uint8_t lives = 3;
        uint64_t lastDrop = 0;

        struct Rob {
            float xLocation;
            uint8_t robotID;
        };

        std::stack<Rob> robots;

        inline static constexpr const uint8_t VisibleRobotCount = 3;

        std::array<GameObjPtr, VisibleRobotCount> visibleRobots;

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
        void attachRobot(uint8_t robot) const;

        inline static constexpr std::string getRobotPath(uint8_t robot) {
            if(robot > 6){
                return "";
            }

            return "/robot" + std::to_string(robot + 1) + ".raw";
        }
	};
}

#endif //BIT_FIRMWARE_WACKYSTACKY_H