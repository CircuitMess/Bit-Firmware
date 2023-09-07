#ifndef BIT_FIRMWARE_WINDOWS_H
#define BIT_FIRMWARE_WINDOWS_H

#include "GameEngine/GameObject.h"
#include <functional>

class Windows {
public:
	Windows(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile, std::function<void()> hitGood, std::function<void()> hitBad);

	void loop(float dt);

	bool hit(glm::ivec2 pos);

private:
	std::vector<GameObjPtr> chars;
	uint8_t charLoc[3];
	float timeOffsets[3] = {0};

	bool alive[3] = { true, true, true };
	File files[3];
	float dieT[3] = {0};

	enum { Up, Dropping, Rising } state = Rising;
	float T = 0;
	static constexpr float SpeedMove = 1.3f;
	static constexpr float SpeedWait = 0.3f;

	void relocChars();
	void randOffsets();
	void repos();

	std::function<void()> hitGood;
	std::function<void()> hitBad;

	bool allDead();

	template<typename T>
	static constexpr T easeOutExp(T x){
		return x == 1 ? 1 : 1.0 - std::pow(2.0, -10.0 * (double) x);
	}

	template<typename T>
	static constexpr T easeInQuad(T x){
		return std::pow((double) x, 2.0);
	}

	template<typename T>
	static constexpr T easeInBack(T x){
		const double c1 = 1.70158;
		const double c3 = c1 + 1;
		return c3 * std::pow((double) x, 3.0) - c1 * std::pow((double) x, 2.0);
	}
};


#endif //BIT_FIRMWARE_WINDOWS_H
