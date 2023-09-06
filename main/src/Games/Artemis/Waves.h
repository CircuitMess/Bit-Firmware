#ifndef BIT_FIRMWARE_WAVES_H
#define BIT_FIRMWARE_WAVES_H

#include <functional>
#include "GameEngine/GameObject.h"

class Waves {
public:
	Waves(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile);

	void loop(float dt);

	bool hitFront(glm::ivec2 pos);

private:
	GameObjPtr waveBack;
	GameObjPtr waveFront;

	float T = 0;
	float dir = 1;

	static constexpr float Speed = 0.8;

	File fileFrontWave;

	template<typename T>
	static constexpr T easeInOutQuad(T x){
		return x < 0.5 ? 2.0 * x * x : 1.0 - pow(-2.0 * x + 2.0, 2.0) / 2.0;
	}

};


#endif //BIT_FIRMWARE_WAVES_H
