#ifndef BIT_FIRMWARE_PLAYER_H
#define BIT_FIRMWARE_PLAYER_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Devices/Input.h"

namespace CapacitronGame {
class CapacitronGame;

class Player {
public:
	Player(GameObjPtr playerObj, GameObjPtr playerLegsObj, CapacitronGame* game, File jumpFile, File deadFile, File invincFile);

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

	bool isInvincible() const;
	bool isDead() const;
private:
	CapacitronGame* gamePtr;
	GameObjPtr obj;
	GameObjPtr legsObj;

	static constexpr float HorizontalSpeed = 60;
	float ySpeed = 0;
	int8_t horizontalDirection = 0;

	float invincibilityTime = 0;
	static constexpr float InvincibilityBlinkDuration = 0.1f;
	static constexpr float InvincibilityDuration = 1.5f;
	static constexpr float PotionDuration = 5.0f;

	File jumpFile;
	File deadFile;
	File invincFile;
	std::shared_ptr<AnimRC> anim;

	enum class State {
		Jumping, Damaged, Death, Invincibility
	} state = State::Jumping;

	Input::Button lastPressed = Input::Menu;

	void updateMovement(float delta);
	void updateState(float delta);
};

}

#endif //BIT_FIRMWARE_PLAYER_H
