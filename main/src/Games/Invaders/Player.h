#ifndef BIT_FIRMWARE_INVADERS_PLAYER_H
#define BIT_FIRMWARE_INVADERS_PLAYER_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Devices/Input.h"

namespace Invaders {

class Invaders;

class Player {
public:
	Player(GameObjPtr playerObj, Invaders* game, File hitFile, File deadFile);
	void update(float delta);
	void btnPressed(Input::Button btn);
	void btnReleased(Input::Button btn);

	void damage();
	void death();

	bool isDead() const;


private:
	Invaders* gamePtr;
	GameObjPtr obj;
	std::shared_ptr<AnimRC> anim;

	static constexpr float HorizontalSpeed = 60;
	float ySpeed = 0;
	int8_t horizontalDirection = 0;

	enum class State {
		Idle, Damaged, Death
	} state = State::Idle;

	float invincibilityTime = 0;
	static constexpr float HitAnimDuration = 0.24;
	static constexpr uint8_t HitAnimLoops = 4;
	static constexpr float InvincibilityDuration = HitAnimDuration * HitAnimLoops;

	File hitFile;
	File deadFile;

	Input::Button lastPressed = Input::Menu;

	void updateMovement(float delta);
	void updateState(float delta);

};
}


#endif //BIT_FIRMWARE_INVADERS_PLAYER_H
