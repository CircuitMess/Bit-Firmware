#include "CharlieGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"

CharlieGame::CharlieGame::CharlieGame(Sprite& base) : Game(base, Games::Charlie, "/Games/Charlie", {
		RES_GOBLET,
		RES_HEART,
		{ "/bg.raw", {}, true },
		{ "/net.raw", {}, true },
		{ "/cac.gif", {}, false },
		{ "/puf.gif", {}, false },
		{ "/ch_idle.gif", {}, false },
		{ "/ch_roll.gif", {}, false },
		{ "/ch_walk.gif", {}, false },
		{ "/fly_fly.gif", {}, false },
		{ "/fly_plot.gif", {}, false },
		{ "/fly_unroll.gif", {}, false }
}){

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
	return 0;
}

void CharlieGame::CharlieGame::handleInput(const Input::Data& data){
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
	flySpawnT += deltaTime;
	if(flySpawnT >= FlySpawnRate){
		flySpawnT = 0;

		auto fly = new Fly([this](const char* name){ return getFile(name); });
		addObject(*fly);
		flies.emplace(fly);
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
	go->getRenderComponent()->setVisible(false);

	auto cac = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/cac.gif")),
			nullptr
	);
	cac->setPos(go->getPos() + glm::vec2 { 3.5f, 18.0f });
	auto rc = (AnimRC*) cac->getRenderComponent().get();
	rc->setLayer(0);
	rc->start();

	addObject(cac);
	cacs.emplace_back(Cacoon { cac, 0, rollingFly });

	rolling = false;
	rollingFly = nullptr;
	chrl->setRoll(false);
}

void CharlieGame::CharlieGame::updateCacs(float dt){
	std::vector<Cacoon> forRemoval;

	for(auto& cac : cacs){
		cac.t += dt;
		if(cac.t >= CacoonTime){
			// TODO: score++ sound
			score++;
			scoreEl->setScore(score);

			auto puf = std::make_shared<GameObject>(
					std::make_unique<AnimRC>(getFile("/puf.gif")),
					nullptr
			);
			puf->setPos(cac.go->getPos() + glm::vec2 { -7.0f, -12.0f });
			auto rc = (AnimRC*) puf->getRenderComponent().get();
			rc->setLayer(2);
			rc->setLoopMode(GIF::Single);
			rc->start();

			addObject(puf);
			pufs.emplace_back(Puf { puf, 0 });

			forRemoval.emplace_back(cac);
		}
	}

	for(const auto& cac : forRemoval){
		removeObject(cac.go);
		cac.fly->done();
		std::erase_if(cacs, [cac](const Cacoon& other){ return other.go == cac.go; });
	}
}

void CharlieGame::CharlieGame::updateFlies(float dt){
	std::unordered_set<Fly*> forRemoval;

	for(const auto fly : flies){
		fly->update(dt);
		if(fly->isDone()){
			forRemoval.emplace(fly);
		}
	}

	for(auto fly : forRemoval){
		flies.erase(fly);
		removeObject(*fly);
		delete fly;
	}
}

void CharlieGame::CharlieGame::updatePufs(float dt){
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

	for(const auto fly : flies){
		if(!fly->isPlotting()) continue;
		const float dist = glm::length(((GameObjPtr) *fly)->getPos() + Fly::SpriteSize/2.0f - ((GameObjPtr) *chrl)->getPos() - Char::SpriteSize/2.0f);
		closest.push_back(CloseFlies { fly, dist });
	}

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
