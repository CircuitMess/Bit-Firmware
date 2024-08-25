#ifndef BIT_FIRMWARE_DUCK4_H
#define BIT_FIRMWARE_DUCK4_H

#include <memory>
#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Collision/CircleCC.h"

namespace MarvGame {
class MarvGame;

class Player {
public:
	Player(std::shared_ptr<GameObject> duckGoRc, std::shared_ptr<GameObject> duckGoCc, MarvGame* game4);
	~Player();
	void death();
	void update(float deltaTime);

	std::shared_ptr<GameObject> getGameObjectCc();
	void setFiles(File walk, File down, File jump, File ducking, File ducked, File unDucking, File winAnim);
	void win();
	void duckPressed();
	void jumpPressed();
	void duckReleased();

	bool invincible = false;

private:
	void walk();

	void jump();
	void duck();
	void updateInvincibility(float delta);

	MarvGame* gamePtr;
	std::shared_ptr<GameObject> gameObjectRc;
	std::shared_ptr<GameObject> gameObjectCc;
	std::shared_ptr<AnimRC> animRc;

	const int startPosY = 50;
	float multiplier = 1.0f;
	const float maxHeight = -40.0f;
	const float posXForMaxHeight = 60.0f;
	float time = 0.0f;
	float peakTime;
	float velocity;
	float gravity;
	float invincibilityTime = 0;
	float invincibilityBlink = 0;
	float invincibilityBlinkDuration = 0.2f;
	constexpr static float invincibilityDuration = 1.8f;
	bool isJumping = false;
	bool isDucked = false;
	bool winFlag = false;
	bool isDead = false;
	bool duckHold = false;

	File walking;
	File down;
	File jumpAnim;
	File ducking;
	File ducked;
	File unDucking;
	File winAnim;

	File goToAnim;
};
}

#endif //BIT_FIRMWARE_DUCK4_H
