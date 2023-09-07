#include "Game.h"
#include "Ray.h"
#include "GameEngine/Rendering/StaticRC.h"

ArtemisGame::PewPew::PewPew(Sprite& canvas) : Game(canvas, "/Games/Arte", {
		{ "/bg.raw", {}, true },
		{ "/bg_bot.raw", {}, true },
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
		{ "/hit_stick1.gif", {}, true },
		{ "/hit_stick2.gif", {}, true },
		{ "/hit_stick3.gif", {}, true },
		{ "/hit_stick4.gif", {}, true },
		{ "/hit_stick5.gif", {}, true },

		{ "/windows.raw", {}, true },
		{ "/win1.raw", {}, true },
		{ "/win2.raw", {}, true },
		{ "/win3.raw", {}, true },

		{ "/aim.raw", {}, true },
		RES_HEART
}){

}

void ArtemisGame::PewPew::onLoad(){
	// BG, curtains, window backdrop
	auto winBg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/windows.raw"), PixelDim { 82, 21 })
	);
	winBg->getRenderComponent()->setLayer(-3);
	winBg->setPos(23, 30);

	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim { 128, 105 })
	);
	bg->getRenderComponent()->setLayer(-1);

	auto bgBot = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg_bot.raw"), PixelDim { 128, 23 })
	);
	bgBot->getRenderComponent()->setLayer(50);
	bgBot->setPos(0, 105);

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

	addObjects({ winBg, bg, bgBot, curtL, curtR });

	// Sticks, windows, waves
	const auto hg = [this](){ onPos(); };
	const auto hb = [this](){ onNeg(); };

	for(int i = 0; i < 5; i++){
		sticks.emplace_back((OnStick::Char) i, 20 + i*2, [this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); }, hg, hb);
	}

	windows = std::make_unique<Windows>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); }, hg, hb);

	waves = std::make_unique<Waves>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });

	// Crosshair
	xhair = std::make_unique<Crosshair>([this](GameObjPtr obj){ addObject(obj); }, [this](const char* path){ return getFile(path); });

	// Hearts
	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	hearts->getGO()->getRenderComponent()->setLayer(60);
	addObject(hearts->getGO());
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
	if(Ray::within(pos, { 0, 105 }, { 128, 128 })) return;

	for(auto stick = sticks.end()-1; stick >= sticks.begin(); stick--){
		if(stick->hit(pos)) return;
	}

	if(Ray::hitTest(pos, getFile("/bg.raw"), { 128, 128 })) return;
	if(windows->hit(pos)) return;
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

void ArtemisGame::PewPew::onPos(){
	score++;

	if(score >= 6){
		exit();
	}
}

void ArtemisGame::PewPew::onNeg(){
	lives--;
	hearts->setLives(lives);

	if(lives == 0){
		exit();
	}
}
