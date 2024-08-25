#include "Game.h"
#include "Ray.h"
#include "GameEngine/Rendering/StaticRC.h"

ArtemisGame::PewPew::PewPew(Sprite& canvas) : Game(canvas, Games::Artemis, "/Games/Arte", {
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

		{ "/debree.raw", {}, true },
		{ "/aim.raw", {}, true },
		RES_HEART
}){
	robot = std::make_shared<RoboCtrl::Artemis>();
	setRobot(robot);
}

uint32_t ArtemisGame::PewPew::getXP() const{
	return ((float) score / 6.0f) * 150.0f;
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

	// Bullet debree
	debree = std::make_unique<Debree>([this](GameObjPtr obj){ addObject(obj); }, [this](GameObjPtr obj){ removeObject(obj); }, [this](const char* path){ return getFile(path); });

	// Hearts
	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	hearts->getGO()->getRenderComponent()->setLayer(60);
	addObject(hearts->getGO());

	robot->setSpeed(500);
}

void ArtemisGame::PewPew::onStart(){
	Game::onStart();

	xhair->btnReset();
}

void ArtemisGame::PewPew::onLoop(float dt){
	shootTime += dt;

	debree->loop(dt);

	if(roboFastT != 0){
		roboFastT += dt;
		if(roboFastT >= 2){
			roboFastT = 0;
			robot->setSpeed(500.0f * (1.0f - (float) score / 6.0f));
		}
	}

	for(auto& stick : sticks){
		if(done && lives != 0 && stick.getChar() == OnStick::Artemis) continue; // Win condition anim for Artemis
		stick.loop(dt);
	}
	windows->loop(dt);

	if(!done){
		waves->loop(dt);
	}

	xhair->loop(dt);

	if(done){
		doneT += dt;
		if(doneT >= 1.5){
			exit();
		}
	}
}

void ArtemisGame::PewPew::handleInput(const Input::Data& data){
	xhair->btnAction(data.btn, data.action);

	if(data.btn == Input::A && data.action == Input::Data::Press){
		fire();
	}
}

void ArtemisGame::PewPew::fire(){
	if(done) return;

	if(shootTime < ShootCooldown) return;
	shootTime = 0;

	const auto pos = xhair->getAim();

	shootHit = false;
	shootPos = pos;

#define out() { shootSound(); return; }

	if(hitCurtain(pos)) out();
	if(waves->hitFront(pos)) out();
	if(Ray::within(pos, { 0, 105 }, { 128, 128 })) out();

	for(auto stick = sticks.end()-1; stick >= sticks.begin(); stick--){
		if(stick->hit(pos)) out();
	}

	if(Ray::hitTest(pos, getFile("/bg.raw"), { 128, 128 })) out();
	if(windows->hit(pos)) out();

	shootSound();
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
	if(done) return;

	shootHit = true;
	score++;

	roboFastT = 0;
	robot->setSpeed(500.0f * (1.0f - (float) score / 6.0f));

	if(score >= 6){
		audio.play({
		   Chirp { 1600, 1800, 100 },
		   Chirp { 1000, 800, 50 },

		   Chirp { 400, 500, 200 },
		   Chirp { 0, 0, 150 },
		   Chirp { 550, 700, 200 },
		   Chirp { 0, 0, 150 },
		   Chirp { 800, 1000, 200 },
		});

		if(lives == 3){
			addAchi(Achievement::Artemis_sharp, 1);
		}

		finish();
		return;
	}

	audio.play({
		   Chirp { 800, 600, 50 },
		   Chirp { 120, 100, 10 },
		   Chirp { 1600, 1800, 100 },
		   Chirp { 1000, 800, 50 }
   });
}

void ArtemisGame::PewPew::onNeg(){
	if(done) return;

	shootHit = true;
	lives--;
	hearts->setLives(lives);

	addAchi(Achievement::Artemis_friendly, 1);

	roboFastT = 0.001;
	robot->setSpeed(0);

	if(lives == 0){
		roboFastT = 0;

		audio.play({
		   Chirp{ 300, 350, 100 },
		   Chirp{ 350, 1200, 100 },
		   Chirp{ 1200, 1100, 70 },
		   Chirp{ 0, 0, 80 },
		   Chirp{ 1100, 1050, 80 },

		   Chirp { 0, 0, 100 },
		   Chirp { 790, 700, 300 },
		   Chirp { 0, 0, 100 },
		   Chirp { 700, 610, 300 },
		   Chirp { 0, 0, 100 },
		   Chirp { 610, 420, 300 },
		   Chirp { 420, 420, 200 }
		});

		finish();
		return;
	}

	audio.play({
	   Chirp { 800, 600, 50 },
	   Chirp { 120, 100, 10 },
	   Chirp { 0, 0, 50 },

	   Chirp { 300, 350, 100 },
	   Chirp { 350, 1200, 100 },
	   Chirp { 1200, 1100, 70 },
	   Chirp { 0, 0, 80 },
	   Chirp { 1100, 1050, 80 },
	   Chirp { 0, 0, 80 },
	   Chirp { 1050, 1000, 80 },
	   Chirp { 0, 0, 80 },
	   Chirp { 1000, 950, 80 },

	   Chirp { 0, 0, 100 },
	   Chirp { 950, 910, 80 },
	   Chirp { 0, 0, 80 },
	   Chirp { 910, 870, 80 },
	   Chirp { 0, 0, 80 },
	   Chirp { 870, 830, 80 },
	   Chirp { 0, 0, 80 },
	   Chirp { 830, 790, 80 },

	   Chirp { 0, 0, 90 },
	   Chirp { 790, 750, 90 },
	   Chirp { 0, 0, 90 },
	   Chirp { 750, 710, 100 }
	});
}

void ArtemisGame::PewPew::finish(){
	if(done) return;
	done = true;

	xhair->hide();

	windows->hide();
	for(int i = 0; i < 4; i++){
		sticks[i].hide();
	}

	if(lives == 0){ // Lose (as opposed to win)
		sticks[4].hide(); // Artemis death anim
	}
}

void ArtemisGame::PewPew::shootSound(){
	if(shootHit) return;

	audio.play({
		Chirp { 800, 600, 50 },
		Chirp { 120, 100, 100 }
	});

	debree->place(shootPos);
}
