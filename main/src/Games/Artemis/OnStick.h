#ifndef BIT_FIRMWARE_ONSTICK_H
#define BIT_FIRMWARE_ONSTICK_H

#include "GameEngine/GameObject.h"

class OnStick {
public:
	enum Char { DuckYellow, DuckPurple, DuckSmallYellow, DuckSmallPurple, Artemis };

	OnStick(Char chr, int8_t layer, std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile, std::function<void()> hitGood, std::function<void()> hitBad);

	void loop(float dt);

	bool hit(glm::ivec2 pos);

	Char getChar() const;

private:
	std::shared_ptr<GameObject> objStick;
	std::shared_ptr<GameObject> objChar;

	GameObjPtr objCharArte;
	GameObjPtr objCharArteAnim;
	float arteHitT;

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

	std::function<void()> hitGood;
	std::function<void()> hitBad;

};


#endif //BIT_FIRMWARE_ONSTICK_H
