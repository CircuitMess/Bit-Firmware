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
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;
	inline uint32_t getXP() const override { return score * 3; }
	inline uint32_t getScore() const override { return score; }

private:
	inline static constexpr const uint8_t VerticalTiles = 5;
	inline static constexpr const uint8_t HorizontalTiles = 3;
	inline static constexpr const float Speed = 25.0f;
	inline static constexpr const float BoostSpeed = 45.0f;
	inline static constexpr const float CarSpeed = 55.0f;

	inline static constexpr const char* Obstacles[] = {
			"/cones.raw",
			"/oil.raw",
			"/rail.raw",
			"/ramp.raw",
			"/trash.raw",
			"/boost.raw"
	};

	uint32_t score = 0;
	GameObjPtr car;
	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;
	std::array<GameObjPtr, 14> leftEdge;
	std::array<GameObjPtr, 14> rightEdge;
	std::array<GameObjPtr, VerticalTiles * HorizontalTiles> road;
	bool lastGenerated = true;
	uint8_t rowToGenerate = 0;
	float direction = 0.0f;

private:
	void generateRoad();
};
} // Planck

#endif //BIT_FIRMWARE_PLANCK_H