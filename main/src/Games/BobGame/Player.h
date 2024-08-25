#ifndef CIRCUITPET_FIRMWARE_DUCK_H
#define CIRCUITPET_FIRMWARE_DUCK_H

#include "GameEngine/GameObject.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "Devices/Input.h"

namespace BobGame {
class BobGame;

class Player {
public:
	Player(File eat, File eatBad, File win, File dead);
	~Player();
	std::shared_ptr<GameObject> getGameObject();
	void loop(float deltaTime);
	void startEating(int value);
	void filled(BobGame* game);
	void killed(BobGame* game);

	bool isEatingBad() const;
	void btnPressed(Input::Button btn);
	void btnReleased(Input::Button btn);

private:
	void finishEating();
	const uint16_t width = 69;
	const uint16_t height = 71;
	float velocity = 0.0f;
	const float speed = 60.0f;
	const float collisionRadius = 19;
	const glm::vec2 collisionOffset = glm::vec2{ 38, 38 };
	bool eating = false;
	bool eatingBad = false;
	bool eatDone = false;
	Input::Button lastPressed = Input::Menu;

	File eat;
	File eatBad;
	File win;
	File dead;

	std::shared_ptr<GameObject> go;
	std::shared_ptr<AnimRC> anim;
};

}
#endif //CIRCUITPET_FIRMWARE_DUCK_H
