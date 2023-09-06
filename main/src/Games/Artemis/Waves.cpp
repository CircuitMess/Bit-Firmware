#include "Waves.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "Ray.h"

Waves::Waves(std::function<void(GameObjPtr)> addObject, std::function<File(const char* path)> getFile){
	waveBack = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/wave_back.raw"), PixelDim { 116, 11 })
	);
	waveBack->getRenderComponent()->setLayer(7);
	waveBack->setPos(12, 94); // startX: endBack in moveWaves()
	addObject(waveBack);

	fileFrontWave = getFile("/wave_front.raw");
	waveFront = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(fileFrontWave, PixelDim { 125, 13 })
	);
	waveFront->getRenderComponent()->setLayer(50);
	waveFront->setPos(-4, 92); // startX: startFront in moveWaves()
	addObject(waveFront);
}

bool Waves::hitFront(glm::ivec2 pos){
	const auto inside = Ray::within(pos, waveFront->getPos(), waveFront->getPos() + glm::vec2(125, 13));
	if(!inside) return false;

	return Ray::hitTest(pos - glm::ivec2(waveFront->getPos()), fileFrontWave, PixelDim { 125, 13 });
}

void Waves::loop(float dt){
	static constexpr float startBack = 0;
	static constexpr float endBack = 12;
	static constexpr float startFront = -4;
	static constexpr float endFront = 8;

	T += Speed * dt * dir;
	if(T >= 1.0f || T <= 0.0f){
		dir *= -1;

		if(T < 0){
			T = -T;
		}else if(T > 1){
			T = 2.0f - T;
		}
	}

	const float t = easeInOutQuad(T);
	waveFront->setPosX(startFront + (endFront - startFront) * t);
	waveBack->setPosX(startBack + (endBack - startBack) * (1.0f - t));
}
