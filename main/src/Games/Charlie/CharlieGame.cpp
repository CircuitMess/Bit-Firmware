#include "CharlieGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include <esp_random.h>
#include "Util/stdafx.h"

CharlieGame::CharlieGame::CharlieGame(Sprite& base) : Game(base, Games::Charlie, "/Games/Charlie", {
		RES_GOBLET,
		RES_HEART,
		{ "/bg.raw", {}, true },
		{ "/net.raw", {}, true },
		{ "/cac.gif", {}, true },
		{ "/puf.gif", {}, true },
		{ "/ch_idle.gif", {}, true },
		{ "/ch_roll.gif", {}, true },
		{ "/ch_walk.gif", {}, true },
		{ "/fly_fly.gif", {}, true },
		{ "/fly_plot.gif", {}, true },
		{ "/fly_unroll.gif", {}, true }
}){

}

CharlieGame::CharlieGame::~CharlieGame(){
	cacs.iterate([this](Cacoon* cac){
		cacs.rem(cac);
		delete cac;
	});

	flies.iterate([this](Fly* fly){
		flies.rem(fly);
		delete fly;
	});
}

void CharlieGame::CharlieGame::onLoad(){
	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-2);
	addObject(bg);

	auto net = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/net.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	net->getRenderComponent()->setLayer(-1);
	addObject(net);

	livesEl = std::make_unique<Hearts>(getFile(FILE_HEART));
	livesEl->getGO()->setPos({ 2, 2 });
	livesEl->getGO()->getRenderComponent()->setLayer(10);
	livesEl->setLives(lives);
	addObject(livesEl->getGO());

	scoreEl = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreEl->getGO()->setPos({ 128 - 28 - 2, 2 });
	scoreEl->getGO()->getRenderComponent()->setLayer(10);
	scoreEl->setScore(score);
	addObject(scoreEl->getGO());

	chrl = std::make_unique<Char>([this](const char* name){ return getFile(name); });
	addObject(*chrl);
}

uint32_t CharlieGame::CharlieGame::getXP() const{
	return score * 3;
}

uint32_t CharlieGame::CharlieGame::getScore() const{
	return score;
}

void CharlieGame::CharlieGame::handleInput(const Input::Data& data){
	if(over) return;

	if(data.action == Input::Data::Press){
		chrl->btnPress(data.btn);
	}else{
		chrl->btnRelease(data.btn);
	}

	if(data.btn == Input::A){
		if(data.action == Input::Data::Press){
			startRoll();
		}else{
			stopRoll();
		}
	}
}

void CharlieGame::CharlieGame::onLoop(float deltaTime){
	if(over){
		updateFlies(deltaTime);
		updateOver(deltaTime);
		return;
	}

	flySpawnT += deltaTime;
	if(flies.count() < MaxFlies/2 && flySpawnT >= FlySpawnRate / std::min(3.0f, std::max(1.0f, (float) score / 7.0f))){
		flySpawnT = 0;

		auto fly = new Fly([this](const char* name){ return getFile(name); }, nullptr, [this](Cacoon* cac){
			if(over) return;
			// TODO: fly left sound (before dmg call)
			dmg();
		});

		addObject(*fly);
		if(!flies.add(fly)){ printf("Fail!!!\n"); }
	}

	updateRoll(deltaTime);
	chrl->update(deltaTime);

	updateCacs(deltaTime);
	updateFlies(deltaTime);

	updatePufs(deltaTime);
}

void CharlieGame::CharlieGame::updateRoll(float dt){
	if(!rolling) return;

	rollT += dt;
	if(rollT < RollTime) return;

	auto go = (GameObjPtr) *rollingFly;

	auto cacGo = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/cac.gif"), true),
			nullptr
	);
	cacGo->setPos(go->getPos() + Fly::SpriteSize * 0.5f - glm::vec2 { 14, 17 } * 0.5f);
	auto rc = (AnimRC*) cacGo->getRenderComponent().get();
	rc->setLayer(0);
	rc->start();

	addObject(cacGo);

	auto cac = new Cacoon(cacGo, 0, rollingFly, false, nullptr);
	cacs.add(cac);

	if((esp_random() % 100) < (int) std::round(50.0f + std::min(50.0f, map((float) score, 0, 20, 0, 50))) && flies.count() < MaxFlies && score >= 2){
		cac->beingRescued = true;
	}

	rolling = false;
	rollingFly = nullptr;
	chrl->setRoll(false);

	cacoons++;

	// TODO: fly rolled sound
}

void CharlieGame::CharlieGame::updateCacs(float dt){
	if(cacs.count() == 0) return;

	cacs.iterate([this, dt](Cacoon* cac){
		if(cac->beingRescued && cac->rescuer) return;

		cac->t += dt;
		if(cac->t >= CacoonTime){
			// TODO: score++ sound
			score++;
			scoreEl->setScore(score);

			auto puf = std::make_shared<GameObject>(
					std::make_unique<AnimRC>(getFile("/puf.gif"), true),
					nullptr
			);
			puf->setPos(cac->go->getPos() + glm::vec2 { -8.0f, -12.0f });
			auto rc = (AnimRC*) puf->getRenderComponent().get();
			rc->setLayer(2);
			rc->setLoopMode(GIF::Single);
			rc->start();

			addObject(puf);
			pufs.emplace_back(Puf { puf, 0 });

			cac->fly->done();

			removeObject(cac->go);
			cacs.rem(cac);
			delete cac;
		}else if(cac->beingRescued && cac->t >= CacoonTime/2.0f && cac->rescuer == nullptr){
			auto fly = new Fly([this](const char* name){ return getFile(name); }, cac, [this](Cacoon* cac){
				if(over) return;

				if(cac){
					removeObject(cac->go);
					cacs.rem(cac);
					delete cac;
				}

				// TODO: fly rescued sound (before dmg call)

				escapes++;

				dmg();
			});
			addObject(*fly);
			if(!flies.add(fly)){ printf("Fail!!!\n"); }

			cac->rescuer = fly;
		}
	});
}

void CharlieGame::CharlieGame::updateFlies(float dt){
	if(flies.count() == 0) return;

	flies.iterate([this, dt](Fly* fly){
		if(fly == nullptr) return;

		if(fly->isRescuing()){
			const float dist = glm::length(((GameObjPtr) *fly)->getPos() + Fly::SpriteSize * 0.5f - ((GameObjPtr) *chrl)->getPos() - Char::SpriteSize * 0.5f);
			if(dist <= 30.0f){
				fly->goAway();
			}
		}

		fly->update(dt);
		if(fly->isDone()){
			removeObject(*fly);
			flies.rem(fly);
			delete fly;
		}
	});
}

void CharlieGame::CharlieGame::updatePufs(float dt){
	if(pufs.empty()) return;

	std::vector<Puf> forRemoval;

	for(auto& puf : pufs){
		puf.t += dt;
		if(puf.t >= PufDuration){
			forRemoval.emplace_back(puf);
		}
	}

	for(const auto& puf : forRemoval){
		removeObject(puf.go);
		std::erase_if(pufs, [puf](const Puf& other){ return other.go == puf.go; });
	}
}

void CharlieGame::CharlieGame::startRoll(){
	if(rolling) return;

	struct CloseFlies {
		Fly* fly;
		float dist;
	};
	std::vector<CloseFlies> closest;

	flies.iterate([this, &closest](Fly* fly){
		if(!fly->isPlotting()) return;
		const float dist = glm::length(((GameObjPtr) *fly)->getPos() + Fly::SpriteSize * 0.5f - ((GameObjPtr) *chrl)->getPos() - Char::SpriteSize * 0.5f);
		closest.push_back(CloseFlies { fly, dist });
	});

	if(closest.empty()) return;

	std::sort(closest.begin(), closest.end(), [](const CloseFlies& a, const CloseFlies& b){ return a.dist < b.dist; });

	if(closest.front().dist > 30.0f){
		return;
	}

	rolling = true;
	rollT = 0;
	chrl->setRoll(true);
	rollingFly = closest.front().fly;
	rollingFly->goCac();

	// TODO: roll start sound
}

void CharlieGame::CharlieGame::stopRoll(){
	if(!rolling) return;
	rolling = false;
	chrl->setRoll(false);

	if(rollingFly){
		rollingFly->goAway();
		rollingFly = nullptr;
	}
}

void CharlieGame::CharlieGame::dmg(){
	lives--;
	livesEl->setLives(lives);

	if(lives == 0){
		gameOver();
	}
}

void CharlieGame::CharlieGame::gameOver(){
	// TODO: game over audio

	over = true;

	chrl->setStop(true);
	stopRoll();

	flies.iterate([](Fly* fly){
		fly->goAway();
	});

	if(escapes == cacoons - cacs.count()){
		// addAchi(Achievement::Charlie_yesmercy, 1);
	}

	cacs.iterate([this](Cacoon* cac){
		removeObject(cac->go);
		cacs.rem(cac);
		delete cac;
	});
}

void CharlieGame::CharlieGame::updateOver(float dt){
	overT += dt;

	if(overT >= OverTimeout){
		exit();
	}
}
