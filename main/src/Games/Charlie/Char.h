#ifndef BIT_FIRMWARE_CHAR_H
#define BIT_FIRMWARE_CHAR_H

#include <GameEngine/GameObject.h>
#include "Devices/Input.h"

namespace CharlieGame {

class Char {
public:
	Char(std::function<File(const char*)> getFile);

	operator GameObjPtr();

	void btnPress(Input::Button btn);
	void btnRelease(Input::Button btn);

	void update(float dt);

	void setRoll(bool rolling);
	void setStop(bool stop);

	static constexpr glm::vec2 SpriteSize = { 57, 51 };

private:
	const std::function<File(const char*)> getFile;
	GameObjPtr go;

	bool started = false;
	glm::vec2 moveDir = { 0, 0 };
	static constexpr float MoveSpeed = 60.0f;

	bool rolling = false;
	bool stopped = false;

	void updateAnim();

};

}


#endif //BIT_FIRMWARE_CHAR_H
