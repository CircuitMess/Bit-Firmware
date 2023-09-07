#ifndef BIT_FIRMWARE_ONSTICK_H
#define BIT_FIRMWARE_ONSTICK_H

#include "GameEngine/GameObject.h"

class OnStick {
public:
	enum Char { DuckYellow, DuckPurple, DuckSmallYellow, DuckSmallPurple, Artemis };

	OnStick(Char chr, int8_t layer, std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile);

	void loop(float dt);

	bool hit(glm::ivec2 pos);

private:
	std::shared_ptr<GameObject> objStick;
	std::shared_ptr<GameObject> objChar;

	const Char chr;
	const int8_t layer;
	File fileChar;
	File fileAnimChar;
	const std::function<void(GameObjPtr)> addObject;

	const glm::ivec2 charOffset;
	float stickStartY;

	float T;
	float dir = 1;
	const float MoveSpeed;
	static constexpr float DropSpeed = 1.5f;
	static constexpr float DropSize = 20;

	void updatePos();
	void updateDropPos();

	enum { Alive, Drop, Dead } state = Alive;

};


#endif //BIT_FIRMWARE_ONSTICK_H
