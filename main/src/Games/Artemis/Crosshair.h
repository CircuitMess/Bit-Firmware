#ifndef BIT_FIRMWARE_CROSSHAIR_H
#define BIT_FIRMWARE_CROSSHAIR_H

#include "GameEngine/GameObject.h"
#include "Devices/Input.h"
#include <unordered_set>

class Crosshair {
public:
	Crosshair(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile);

	void loop(float dt);

	void btnAction(Input::Button btn, Input::Data::Action action);
	void btnReset();

	glm::ivec2 getAim();

private:

	GameObjPtr sight;

	glm::vec2 sightDir = {};

	double sineTime = 0;
	glm::vec2 sinePos = {};

	enum AimDir { Up, Down, Left, Right };
	static const std::unordered_map<Input::Button, AimDir> BtnAim;
	std::unordered_set<AimDir> pressedAim;

	glm::vec2 getAimDir();

};


#endif //BIT_FIRMWARE_CROSSHAIR_H
