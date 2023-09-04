#include "Game.h"
#include "GameEngine/Rendering/StaticRC.h"

ArtemisGame::PewPew::PewPew(Sprite& canvas) : Game(canvas, "/Games/Arte", {
		{ "/bg.raw", {}, true },
		{ "/curt_l.raw", {}, true },
		{ "/curt_r.raw", {}, true },
		{ "/wave_front.raw", {}, true },
		{ "/wave_back.raw", {}, true },

		{ "/stick.raw", {}, true },
		{ "/stick1.raw", {}, true },
		{ "/stick2.raw", {}, true },
		{ "/stick3.raw", {}, true },
		{ "/stick4.raw", {}, true },
		{ "/stick5.raw", {}, true },

}){

}

void ArtemisGame::PewPew::onLoad(){
	// BG and curtains
	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim { 128, 128 })
	);
	bg->getRenderComponent()->setLayer(-1);

	auto curtL = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/curt_l.raw"), PixelDim { 8, 13 })
	);
	curtL->getRenderComponent()->setLayer(11);
	curtL->setPos(0, 92);

	auto curtR = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/curt_r.raw"), PixelDim { 7, 13 })
	);
	curtR->getRenderComponent()->setLayer(11);
	curtR->setPos(121, 92);

	addObjects({ bg, curtL, curtR });

	// Waves
	waveBack = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/wave_back.raw"), PixelDim { 116, 11 })
	);
	waveBack->getRenderComponent()->setLayer(7);
	waveBack->setPos(12, 94); // startX: endBack in moveWaves()

	waveFront = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/wave_front.raw"), PixelDim { 125, 13 })
	);
	waveFront->getRenderComponent()->setLayer(10);
	waveFront->setPos(-4, 92); // startX: startFront in moveWaves()

	addObjects({ waveFront, waveBack });

	// Sticks
	for(int i = 0; i < 5; i++){
		addStick((OnStick::Char) i);
	}
}

void ArtemisGame::PewPew::addStick(OnStick::Char chr){
	sticks.emplace_back(chr, [this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });
}

void ArtemisGame::PewPew::onLoop(float dt){
	moveWaves(dt);

	for(auto& stick : sticks){
		stick.loop(dt);
	}
}

void ArtemisGame::PewPew::moveWaves(float dt){
	static constexpr float startBack = 0;
	static constexpr float endBack = 12;
	static constexpr float startFront = -4;
	static constexpr float endFront = 8;
	static constexpr float speed = 0.8;

	waveT += speed * dt * waveDir;
	if(waveT >= 1.0f || waveT <= 0.0f){
		waveDir *= -1;

		if(waveT < 0){
			waveT = -waveT;
		}else if(waveT > 1){
			waveT = 2.0f - waveT;
		}
	}

	const float t = easeInOutQuad(waveT);
	waveFront->setPosX(startFront + (endFront - startFront) * t);
	waveBack->setPosX(startBack + (endBack - startBack) * (1.0f - t));
}
