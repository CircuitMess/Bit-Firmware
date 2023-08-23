#ifndef BIT_FIRMWARE_PLAYER_H
#define BIT_FIRMWARE_PLAYER_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Devices/Input.h"

namespace CapacitronGame {
class CapacitronGame;

class Player {
public:
	Player(GameObjPtr playerObj, GameObjPtr playerLegsObj, CapacitronGame* game);

	float update(float delta); //used for updating movement and animations, returns y-axis movement used for camera movement
	void btnPressed(Input::Button btn);
	void btnReleased(Input::Button btn);
	void damage(); //called when taking damage from obstacles
	void fallDown(); //called when missing pads and falling down
	void death(); //called when lives = 0
	void invincible(); //picked up invulnerability potion

	void jump(); //called when landing on a platform
	void trampolineJump(); //called when landing on a trampoline
	float getYSpeed() const;
private:
	CapacitronGame* gamePtr;
	GameObjPtr obj;
	GameObjPtr legsObj;

	static constexpr float HorizontalSpeed = 60;
	float ySpeed = 0;
	int8_t horizontalDirection = 0;

	File jumpFile;
	std::shared_ptr<AnimRC> anim;

	enum class State{
		Jumping, BoostJump, Damaged, Death
	} state;

	Input::Button lastPressed = Input::Menu;
};

}

#endif //BIT_FIRMWARE_PLAYER_H
