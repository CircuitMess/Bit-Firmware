#ifndef BIT_FIRMWARE_ARTEGAME_H
#define BIT_FIRMWARE_ARTEGAME_H

#include "GameEngine/Game.h"

namespace ArtemisGame {

class PewPew : public ::Game {
public:
	PewPew(Sprite& canvas);

private:

	void onLoad() override;
	void onLoop(float deltaTime) override;

	std::shared_ptr<GameObject> waveFront;
	std::shared_ptr<GameObject> waveBack;
	float waveT = 0;
	float waveDir = 1;
	void moveWaves(float dt);

	template<typename T>
	static constexpr T easeInOutCubic(T x){
		return x < 0.5 ? 4.0 * x * x * x : 1.0 - pow(-2.0 * x + 2.0, 3.0) / 2.0;
	}

	template<typename T>
	static constexpr T easeInOutQuad(T x){
		return x < 0.5 ? 2.0 * x * x : 1.0 - pow(-2.0 * x + 2.0, 2.0) / 2.0;
	}
};

}


#endif //BIT_FIRMWARE_ARTEGAME_H
