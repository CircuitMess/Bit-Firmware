#include "HertzGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"

HertzGame::HertzGame(Sprite& canvas) : Game(canvas, "/Games/Hertz", {
		{ "/Arrow.raw", {}, true },
		{ "/bg.raw",    {}, true },
		{ "/win.gif",   {}, false },
		{ "/blink.gif", {}, false },
		{ "/idle.gif",  {}, true },

}){}

void HertzGame::onLoad(){
	auto spriteBar = std::make_unique<SpriteRC>(PixelDim{ 114, 6 });
	bar = std::make_unique<Bar>(spriteBar->getSprite());
	bar->resetGoal();
	barGO = std::make_shared<GameObject>(
			std::move(spriteBar),
			nullptr
	);
	addObject(barGO);
	barGO->getRenderComponent()->setLayer(1);
	barGO->setPos({ 7, 43 });

	indicatorGO = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/Arrow.raw"), PixelDim{ 5, 7 }),
			nullptr
	);
	addObject(indicatorGO);
	indicatorGO->getRenderComponent()->setLayer(1);
	indicator = std::make_unique<Indicator>(indicatorGO);
	indicator->setGoal(bar->getY());

	auto pBarRC = std::make_unique<SpriteRC>(PixelDim{ 114, 7 });
	progressBar = std::make_unique<ProgressBar>(pBarRC->getSprite());
	progressGO = std::make_shared<GameObject>(
			std::move(pBarRC),
			nullptr
	);
	addObject(progressGO);
	progressGO->setPos({ 7, 55 });
	progressGO->getRenderComponent()->setLayer(1);

	mainGO = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/idle.gif")),
			nullptr
	);
	addObject(mainGO);
	mainGO->getRenderComponent()->setLayer(1);
	mainGO->setPos({ 9, 72 });
	duckAnim = std::static_pointer_cast<AnimRC>(mainGO->getRenderComponent());

	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim({ 128, 128 })),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(0);
}

void HertzGame::onLoop(float deltaTime){
	if(done){
		winCounter += deltaTime;
		if(winCounter >= WinPause){
			exit();
			return;
		}
	}else{
		indicator->move(deltaTime);
	}
}

void HertzGame::onStart(){
	duckAnim->start();

}

void HertzGame::onStop(){
	duckAnim->stop();
}

void HertzGame::resetAnim(){
	duckAnim->setAnim(getFile("/idle.gif"));
	duckAnim->setLoopMode(GIF::Single);
}

void HertzGame::addPoints(int difference){
	multiplier = (length - (float) difference) / length;
	multiplier = pow(multiplier, 12);
	fillPercent = std::clamp(fillPercent + multiplier * maxPoints, 0.0f, 1.0f);
	progressBar->fill(fillPercent);
	bar->resetGoal();

	if(indicator->getDifference() < 30){
		audio.play({ { 250, 200, 50 },
					 { 400, 700, 50 } });
	}else if(indicator->getDifference() >= 30){
		audio.play({ { 300, 300, 50 },
					 { 0,   0,   50 },
					 { 300, 300, 50 } });
	}

	if(fillPercent >= 0.999f){
		Sound s = { { 600, 400,  200 },
					{ 400, 1000, 200 } };
		audio.play(s);
//		removeObject(barGO);
//		removeObject(indicatorGO);
		duckAnim->setAnim(getFile("/win.gif"));
		duckAnim->setLoopMode(GIF::Single);
		done = true;
	}else{
		duckAnim->setAnim(getFile("/blink.gif"));
		duckAnim->setLoopDoneCallback([this](uint32_t){
			resetAnim();
		});

		if(indicator->getDifference() < 30){
			audio.play({ { 250, 200, 50 },
						 { 400, 700, 50 } });
		}else if(indicator->getDifference() >= 30){
			audio.play({ { 300, 300, 50 },
						 { 0,   0,   50 },
						 { 300, 300, 50 } });
		}
	}

	indicator->setGoal(bar->getY());
}

void HertzGame::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;

	if(done){
		exit();
		return;
	}


//	if(data.btn == Input::B){
//		audio.play(Sound{ Chirp{ 400, 350, 50 } });
//		exit();
//		return;
//	}
	if(data.btn == Input::A){
		tries++;
		addPoints(indicator->getDifference());
	}
}
