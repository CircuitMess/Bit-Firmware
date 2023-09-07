#ifndef BIT_FIRMWARE_DEBREE_H
#define BIT_FIRMWARE_DEBREE_H

#include "GameEngine/GameObject.h"
#include <functional>
#include <deque>

class Debree {
public:
	Debree(std::function<void(GameObjPtr)> addObject, std::function<void(GameObjPtr)> removeObject, std::function<File(const char* path)> getFile);

	void loop(float dt);

	void place(glm::ivec2 pos);

private:
	std::function<void(GameObjPtr)> addObj;
	std::function<void(GameObjPtr)> remObj;
	std::function<File(const char* path)> getFile;

	struct Deb {
		GameObjPtr obj;
		glm::vec2 velocity;
	};

	static constexpr uint8_t Limit = 6;
	std::deque<Deb> objs;

};


#endif //BIT_FIRMWARE_DEBREE_H
