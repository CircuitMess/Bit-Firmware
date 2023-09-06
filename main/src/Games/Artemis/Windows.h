#ifndef BIT_FIRMWARE_WINDOWS_H
#define BIT_FIRMWARE_WINDOWS_H

#include "GameEngine/GameObject.h"
#include <functional>

class Windows {
public:
	Windows(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile);

	void loop(float dt);

	bool hit(glm::ivec2 pos);

private:
	std::vector<GameObjPtr> chars;
	uint8_t charLoc[3];
	float timeOffsets[3] = {0};

	bool alive[3] = { true, true, true };
	File files[3];

	enum { Up, Dropping, Rising } state = Rising;
	float T = 0;
	static constexpr float SpeedMove = 1.3f;
	static constexpr float SpeedWait = 0.3f;

	void relocChars();
	void randOffsets();
	void repos();

	template<typename T>
	static constexpr T easeOutExp(T x){
		return x == 1 ? 1 : 1.0 - std::pow(2.0, -10.0 * (double) x);
	}

	template<typename T>
	static constexpr T easeInQuad(T x){
		return std::pow((double) x, 2.0);
	}
};


#endif //BIT_FIRMWARE_WINDOWS_H
