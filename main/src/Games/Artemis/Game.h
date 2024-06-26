#ifndef BIT_FIRMWARE_ARTEGAME_H
#define BIT_FIRMWARE_ARTEGAME_H

#include "GameEngine/Game.h"
#include "OnStick.h"
#include "Windows.h"
#include "Waves.h"
#include "Crosshair.h"
#include "Debree.h"
#include "Games/Common/Hearts.h"
#include "RoboCtrl/Artemis.h"

namespace ArtemisGame {

class PewPew : public ::Game {
public:
	PewPew(Sprite& canvas);

protected:
	uint32_t getXP() const override;

private:
	void onLoad() override;
	void onStart() override;

	void onLoop(float deltaTime) override;

	void handleInput(const Input::Data& data) override;

	std::shared_ptr<RoboCtrl::Artemis> robot;
	float roboFastT = 0;

	GameObjPtr bg;
	GameObjPtr curtL, curtR;

	std::vector<OnStick> sticks;
	std::unique_ptr<Windows> windows;
	std::unique_ptr<Waves> waves;
	std::unique_ptr<Debree> debree;

	std::unique_ptr<Crosshair> xhair;
	std::unique_ptr<Hearts> hearts;
	int lives = 3;
	int score = 0;

	static constexpr float ShootCooldown = 0.5f;
	float shootTime = ShootCooldown;
	void fire();
	bool hitCurtain(glm::ivec2 pos);

	void onPos();
	void onNeg();

	bool done = false;
	float doneT = 0;
	void finish();

	glm::vec2 shootPos;
	bool shootHit = false;
	void shootSound();

};

}


#endif //BIT_FIRMWARE_ARTEGAME_H
