#ifndef BIT_FIRMWARE_PLANCK_H
#define BIT_FIRMWARE_PLANCK_H

#include "GameEngine/Game.h"

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

	uint32_t score = 0;
	GameObjPtr car;
	std::array<GameObjPtr, 14> leftEdge;
	std::array<GameObjPtr, 14> rightEdge;
	std::array<GameObjPtr, VerticalTiles * HorizontalTiles> road;
	std::array<bool, 3> lastGenerated = { false, false, false };
};
} // Planck

#endif //BIT_FIRMWARE_PLANCK_H