#ifndef BIT_FIRMWARE_ARTEGAME_H
#define BIT_FIRMWARE_ARTEGAME_H

#include "GameEngine/Game.h"
#include "OnStick.h"
#include "Windows.h"
#include "Waves.h"

namespace ArtemisGame {

class PewPew : public ::Game {
public:
	PewPew(Sprite& canvas);

private:

	void onLoad() override;
	void onLoop(float deltaTime) override;

	std::vector<OnStick> sticks;
	std::unique_ptr<Windows> windows;
	std::unique_ptr<Waves> waves;

};

}


#endif //BIT_FIRMWARE_ARTEGAME_H
