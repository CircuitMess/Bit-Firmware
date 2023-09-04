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

		{ "/windows.raw", {}, true },
		{ "/win1.raw", {}, true },
		{ "/win2.raw", {}, true },
		{ "/win3.raw", {}, true },
}){

}

void ArtemisGame::PewPew::onLoad(){
	// BG, curtains, window backdrop
	auto winBg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/windows.raw"), PixelDim { 82, 21 })
	);
	winBg->getRenderComponent()->setLayer(-3);
	winBg->setPos(23, 30);

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

	addObjects({ winBg, bg, curtL, curtR });

	// Sticks, windows, waves
	for(int i = 0; i < 5; i++){
		sticks.emplace_back((OnStick::Char) i, [this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });
	}

	windows = std::make_unique<Windows>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });

	waves = std::make_unique<Waves>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });
}

void ArtemisGame::PewPew::onLoop(float dt){
	for(auto& stick : sticks){
		stick.loop(dt);
	}

	windows->loop(dt);
	waves->loop(dt);
}
