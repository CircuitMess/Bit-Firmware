#ifndef BIT_FIRMWARE_ONSTICK_H
#define BIT_FIRMWARE_ONSTICK_H

#include "GameEngine/GameObject.h"

class OnStick {
public:
	enum Char { DuckYellow, DuckPurple, DuckSmallYellow, DuckSmallPurple, Artemis };

	OnStick(Char chr, uint8_t layer, std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile);

	void loop(float dt);

	bool hit(glm::ivec2 pos);

private:
	std::shared_ptr<GameObject> objStick;
	std::shared_ptr<GameObject> objChar;

	const Char chr;
	File fileChar;

	float T;
	float dir = 1;
	const float Speed;
	const glm::ivec2 charOffset;

	void updatePos();

	bool alive = true;

};


#endif //BIT_FIRMWARE_ONSTICK_H
