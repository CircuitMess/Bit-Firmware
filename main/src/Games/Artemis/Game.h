#ifndef BIT_FIRMWARE_ARTEGAME_H
#define BIT_FIRMWARE_ARTEGAME_H

#include "GameEngine/Game.h"
#include "OnStick.h"
#include "Windows.h"
#include "Waves.h"
#include "Crosshair.h"

namespace ArtemisGame {

class PewPew : public ::Game {
public:
	PewPew(Sprite& canvas);

private:
	void onLoad() override;
	void onStart() override;

	void onLoop(float deltaTime) override;

	void handleInput(const Input::Data& data) override;

	GameObjPtr curtL, curtR;

	std::vector<OnStick> sticks;
	std::unique_ptr<Windows> windows;
	std::unique_ptr<Waves> waves;

	std::unique_ptr<Crosshair> xhair;

	void fire();
	bool hitCurtain(glm::ivec2 pos);

};

}


#endif //BIT_FIRMWARE_ARTEGAME_H
