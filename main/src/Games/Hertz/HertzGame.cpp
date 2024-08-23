#include "HertzGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"

HertzGame::HertzGame(Sprite& canvas) : Game(canvas, Games::Hertz, "/Games/Hertz", {
		{ "/Arrow.raw", {}, true },
		{ "/bg.raw",    {}, true },
		{ "/win.gif",   {}, false },
		{ "/blink.gif", {}, false },
		{ "/idle.gif",  {}, true },

}){
	robo = std::make_shared<RoboCtrl::Hertz>();
	setRobot(robo);
}

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

	if(animResetQueued){
		animResetQueued = false;
		resetAnim();
	}
}

void HertzGame::onStart(){
	Game::onStart();

	duckAnim->start();
}

void HertzGame::onStop(){
	duckAnim->stop();
}

uint32_t HertzGame::getXP() const{
	if(!done) return 0;

	float success = (float)(MinimumAttempts) /(float)(tries);
	return success * 100.0f;
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

	if(fillPercent >= 0.999f){
		robo->playDone();
		Sound s = { { 400, 300,  100 },
					{ 0,   0,    25 },
					{ 300, 200,  100 },
					{ 0,   0,    25 },
					{ 300, 200,  100 },
					{ 0,   0,    25 },
					{ 800, 1000, 200 } };
		audio.play(s);
//		removeObject(barGO);
//		removeObject(indicatorGO);
		duckAnim->setAnim(getFile("/win.gif"));
		duckAnim->setLoopMode(GIF::Single);
		done = true;
	}else{
		duckAnim->setAnim(getFile("/blink.gif"));
		duckAnim->setLoopMode(GIF::Single);
		duckAnim->setLoopDoneCallback([this](uint32_t){
			animResetQueued = true;
		});

		if(indicator->getDifference() < 30){
			robo->playGood();
			audio.play({ { 80,   800,  100 },
						 { 0,    0,    50 },
						 { 80,   1000, 150 },
						 { 1000, 80,   150 } });

		}else if(indicator->getDifference() >= 30){
			flashAll();
			robo->playBad();
			audio.play({ { 400, 200, 100 },
						 { 0,   0,   100 },
						 { 200,  70,  100 } });
		}
	}

	indicator->setGoal(bar->getY());
}
