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
	uint32_t score = 0;
};
} // Planck

#endif //BIT_FIRMWARE_PLANCK_H