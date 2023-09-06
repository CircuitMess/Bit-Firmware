#include "Game.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "Ray.h"

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

		{ "/aim.raw", {}, true },
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

	curtL = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/curt_l.raw"), PixelDim { 8, 33 })
	);
	curtL->getRenderComponent()->setLayer(51);
	curtL->setPos(0, 72);

	curtR = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/curt_r.raw"), PixelDim { 7, 33 })
	);
	curtR->getRenderComponent()->setLayer(51);
	curtR->setPos(121, 72);

	addObjects({ winBg, bg, curtL, curtR });

	// Sticks, windows, waves
	for(int i = 0; i < 5; i++){
		sticks.emplace_back((OnStick::Char) i, 20 + i*2, [this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });
	}

	windows = std::make_unique<Windows>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });

	waves = std::make_unique<Waves>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });

	// Crosshair
	xhair = std::make_unique<Crosshair>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });
}

void ArtemisGame::PewPew::onStart(){
	xhair->btnReset();
}

void ArtemisGame::PewPew::onLoop(float dt){
	for(auto& stick : sticks){
		stick.loop(dt);
	}

	windows->loop(dt);
	waves->loop(dt);

	xhair->loop(dt);
}

void ArtemisGame::PewPew::handleInput(const Input::Data& data){
	xhair->btnAction(data.btn, data.action);

	if(data.btn == Input::A && data.action == Input::Data::Press){
		fire();
	}
}

void ArtemisGame::PewPew::fire(){
	const auto pos = xhair->getAim();

	if(hitCurtain(pos)) return;
	if(waves->hitFront(pos)) return;

}

bool ArtemisGame::PewPew::hitCurtain(const glm::ivec2 pos){
	const auto inLeft = Ray::within(pos, curtL->getPos(), curtL->getPos() + glm::vec2(8, 33));
	const auto inRight = Ray::within(pos, curtR->getPos(), curtR->getPos() + glm::vec2(7, 33));

	if(!inLeft && !inRight) return false;

	if(inLeft){
		return Ray::hitTest(pos - glm::ivec2(curtL->getPos()), getFile("/curt_l.raw"), PixelDim { 8, 33 });
	}else{
		return Ray::hitTest(pos - glm::ivec2(curtR->getPos()), getFile("/curt_r.raw"), PixelDim { 7, 33 });
	}
}
