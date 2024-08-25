#include "DustyGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"
#include <gtx/rotate_vector.hpp>
#include <esp_random.h>
#include "Util/stdafx.h"

DustyGame::DustyGame::DustyGame(Sprite& canvas) : Game(canvas, Games::Dusty, "/Games/Dusty", {
		RES_GOBLET,
		RES_HEART,
		{ "/bg.raw",         {}, true },
		{ "/char.raw",       {}, true },
		{ "/arm.raw",        {}, true },
		{ "/rat.gif",        {}, true },
		{ ItemsData[0].path, {}, true },
		{ ItemsData[1].path, {}, true },
		{ ItemsData[2].path, {}, true },
		{ ItemsData[3].path, {}, true },
		{ ItemsData[4].path, {}, true },
		{ ItemsData[5].path, {}, true }
}){

}

DustyGame::DustyGame::~DustyGame(){
	ratItems.iterate([this](RatItem* ratItem){
		ratItems.rem(ratItem);
		delete ratItem;
	});

	rats.iterate([this](Rat* rat){
		rats.rem(rat);
		delete rat;
	});

	items.iterate([this](Item* item){
		items.rem(item);
		delete item;
	});
}

uint32_t DustyGame::DustyGame::getXP() const{
	return score * 10;
}

void DustyGame::DustyGame::onLoad(){
	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-2);
	addObject(bg);

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


	auto chr = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/char.raw"), PixelDim{ 37, 29 }),
			nullptr
	);
	chr->setPos(CharPos);
	chr->getRenderComponent()->setLayer(-1);
	addObject(chr);

	auto armRc = std::make_unique<SpriteRC>(PixelDim { 5, 28 });
	armRc->getSprite()->clear(TFT_TRANSPARENT);
	Display::drawFile(*armRc->getSprite(), getFile("/arm.raw"), 0, 14, 5, 14);

	armGo = std::make_shared<GameObject>(
			std::move(armRc),
			std::make_unique<RectCC>(PixelDim { 5, 14 }, PixelDim { 0, 14 })
	);
	armGo->setPos(ArmPos);
	armGo->getRenderComponent()->setLayer(20);
	addObject(armGo);

	const auto wc = [this](){ wallCollision(); };
	collision.wallLeft(*armGo, wc);
	collision.wallRight(*armGo, wc);
	collision.wallBot(*armGo, wc);

	spawnItems();
}

void DustyGame::DustyGame::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;
	if(data.btn != Input::A) return;

	if(lives == 0) return;
	if(ratArm) return;

	if(state == Aiming){
		shoot();
	}
}

void DustyGame::DustyGame::onRender(Sprite& canvas){
	if(state != Shooting && state != Retracting) return;
	if(lives == 0 && !ratArm) return;

	glm::vec2 armLinePos = glm::vec2 { 2, 14 };
	glm::rotate(armLinePos, shootAngl);

	glm::vec2 start = { ArmPos.x + 2, ArmPos.y + 14 };
	glm::vec2 end = { armGo->getPos().x + armLinePos.x, armGo->getPos().y + armLinePos.y };

	if(ratArm && glm::length(ratArm.ropeStartOffset) != 0){
		start = ratArm.rat->go->getPos() + ratArm.ropeStartOffset + glm::vec2 { 2, 14 };
	}

	glm::vec<2, int32_t> _0 = glm::round(start);
	glm::vec<2, int32_t> _1 = glm::round(end);

	canvas.drawLine(_0.x, _0.y, _1.x, _1.y, TFT_BLACK);
}

void DustyGame::DustyGame::onStart(){

}

void DustyGame::DustyGame::onLoop(float deltaTime){
	updateRats(deltaTime);

	if(lives == 0 && rats.count() == 0){
		exit();
		return;
	}

	if(ratArm) return;

	if(state == Aiming){
		updateAim(deltaTime);
	}else if(state == Shooting){
		updateShoot(deltaTime);
	}else if(state == Retracting){
		updateRetract(deltaTime);
	}

	updateSpawn(deltaTime); // unused for now
}

void DustyGame::DustyGame::updateAim(float dt){
	swingT += dt;

	const float rot = 180.0f * std::sin(swingT * 2) / (float) M_PI;
	armGo->setRot(rot);
}

void DustyGame::DustyGame::updateShoot(float dt){
	shootT += dt * 100.0f;
	armGo->setPos(ArmPos + shootDir * shootT);
}

void DustyGame::DustyGame::updateRetract(float dt){
	shootT = std::max(0.0f, shootT - dt * 60.0f);

	const auto newPos = ArmPos + shootDir * shootT;
	armGo->setPos(newPos);

	if(caught){
		caught.item->go->setPos(newPos + caught.offset);
	}

	if(shootT <= 0){
		armGo->setPos(ArmPos);
		state = Aiming;

		if(caught){
			score++;
			scoreEl->setScore(score);

			// TODO: item caught sound
			remCaught();
		}
	}
}

void DustyGame::DustyGame::shoot(){
	if(state != Aiming) return;

	if(spawning) return;

	state = Shooting;
	shootT = 0;

	shootAngl = (float) M_PI * armGo->getRot() / 180.0f;
	shootDir = glm::vec2 { 0, 1 };
	shootDir = glm::rotate(shootDir, shootAngl);
}

void DustyGame::DustyGame::wallCollision(){
	if(state != Shooting) return;
	state = Retracting;
}

void DustyGame::DustyGame::spawnItems(){
	if(spawning) return;
	spawning = true;
	spawnT = 0;

	level++;

	const auto points = randPoints(MaxItems);
	for(int i = 0; i < MaxItems; i++){
		int id = esp_random() % ItemCount;
		if(id == 1 && (esp_random() % 100) > 20){ // Battery
			id = esp_random() % ItemCount;
		}

		const auto& ItemData = ItemsData[id];

		const glm::vec2 randPos = glm::vec2 { 2, 45 } + points[i] * (glm::vec2 { 128 - 2*2, 128 - 45 - 2 } - glm::vec2 { ItemData.size.x, ItemData.size.y });
		const float randRot = ((float) esp_random() / (float) UINT32_MAX) * 120.0f - 60.0f;

		auto go = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(ItemData.path), ItemData.size),
				std::make_unique<RectCC>(ItemData.size)
		);
		go->setPos(randPos);
		go->setRot(randRot);
		go->getRenderComponent()->setLayer(0);
		addObject(go);

		auto item = new DustyGame::Item(go, id);
		items.add(item);

		collision.addPair(*go, *armGo, [this, item](){ itemCollision(item); });
	}

	spawning = false; // TODO: spawn animation
}

void DustyGame::DustyGame::updateSpawn(float dt){
	if(!spawning) return; // TODO: spawn animation
	spawnT += dt;
}

void DustyGame::DustyGame::remCaught(){
	removeObject(caught.item->go);
	items.rem(caught.item);
	delete caught.item;
	caught = {};

	if(items.count() == 0){
		spawnItems();
	}
}

void DustyGame::DustyGame::itemCollision(Item* item){
	if(state != Shooting) return;

	bool ratSnatched = false;
	ratItems.iterate([item, &ratSnatched](RatItem* ratItem){
		if(ratItem->item->item == item) ratSnatched = true;
	});
	if(ratSnatched) return;

	state = Retracting;

	caught = { item, item->go->getPos() - armGo->getPos() };
}

void DustyGame::DustyGame::updateRats(float dt){
	rats.iterate([this, dt](Rat* rat){
		rat->t += dt * rat->speed * (0.1f + 0.15f * std::min(1.0f, (float) score / 40.0f));

		const glm::vec2 newPos = { (float) (128+30) * rat->t - 30, rat->go->getPos().y };
		rat->go->setPos(newPos);

		if(ratArm && ratArm.rat == rat){
			armGo->setPos(rat->go->getPos() + ratArm.offset);
			if(ratArm.rat->go->getPos().y + ratArm.ropeStartOffset.y + 14 > 130.0f){
				if(lives != 0){
					armGo->setPos(ArmPos);
					armGo->setRot(0);
					swingT = 0;
					state = Aiming;

					ratArm = {};
				}
			}
		}

		ratItems.iterate([rat](RatItem* ratItem){
			if(ratItem->rat != rat) return;
			ratItem->item->item->go->setPos(rat->go->getPos() + ratItem->item->offset);
		});

		if(rat->t >= 1){
			if(ratArm && ratArm.rat == rat){
				if(lives != 0){
					armGo->setPos(ArmPos);
					armGo->setRot(0);
					swingT = 0;
					state = Aiming;
				}else{
					armGo->getRenderComponent()->setVisible(false);
				}

				ratArm = {};
			}

			if(ratItems.count() != 0){
				ratItems.iterate([this, rat](RatItem* ratItem){
					if(ratItem->rat != rat) return;

					removeObject(ratItem->item->item->go);
					items.rem(ratItem->item->item);
					delete ratItem->item->item;

					ratItems.rem(ratItem);
					delete ratItem;
				});

				if(items.count() == 0 && lives != 0){
					spawnItems();
				}
			}

			removeObject(rat->go);
			rats.rem(rat);
			delete rat;
		}else if(ratArm.rat == rat && map(ratArm.rat->t, ratArm.startT, 1.0f, 0.0f, 1.0f) >= 0.5f){
			if(ratArm.ropeStartOffset == glm::vec2 { 0, 0 }){
				ratArm.ropeStartOffset = ArmPos - rat->go->getPos();
				ratArm.ropeSpd = -ratArm.ropeStartOffset * 1.0f;
			}

			ratArm.ropeSpd += glm::vec2 { 0, 40.0f } * dt;
			ratArm.ropeStartOffset += ratArm.ropeSpd * dt;
		}
	});

	if(lives == 0) return;

	ratSpawnT += dt;
	if(ratSpawnT >= RatSpawnDelay - 5.5f * std::min(1.0f, (float) score / 30.0f) && rats.count() < MaxRats){
		spawnRat();
		ratSpawnT = 0;
	}
}

void DustyGame::DustyGame::spawnRat(){
	const float y = 40.0f + ((float) esp_random() / (float) UINT32_MAX) * (128.0f - 40.0f - 11.0f);

	auto rc = std::make_unique<AnimRC>(getFile("/rat.gif"), true);
	rc->setLayer(1);
	rc->setLoopMode(GIF::Infinite);
	rc->start();

	// TODO: arm collision: whole rat; item collision: only head
	// current: all collision is on head

	auto go = std::make_shared<GameObject>(
		std::move(rc),
		std::make_unique<RectCC>(PixelDim { 8, 11 }, PixelDim { 22, 0 })
	);
	go->setPos(glm::vec2 { -30, y });
	addObject(go);

	auto rat = new Rat(go, 1.0f + ((float) esp_random() / (float) UINT32_MAX));
	rats.add(rat);

	collision.addPair(*go, *armGo, [this, rat](){ ratHitArm(rat); });

	items.iterate([this, &go, rat](Item* item){
		collision.addPair(*go, *item->go, [this, rat, item](){ ratHitItem(rat, item); });
	});
}

void DustyGame::DustyGame::ratHitItem(Rat* rat, Item* item){
	if(state != Retracting || !caught || caught.item != item) return;

	if(ratArm && ratArm.rat == rat) return;

	bool ratBusy = false;
	ratItems.iterate([&ratBusy, rat](RatItem* ratItem){ if(ratItem->rat == rat) ratBusy = true; });
	if(ratBusy) return;

	auto ratItem = new RatItem(rat, new CaughtItem(caught.item, caught.item->go->getPos() - rat->go->getPos()));
	caught = {};
	ratItems.add(ratItem);

	if(item->id == 1){ // Battery
		addAchi(Achievement::Dusty_rat, 1);
	}

	state = Retracting;
}

void DustyGame::DustyGame::ratHitArm(Rat* rat){
	if(!(state == Shooting || (state == Retracting && !caught && !ratArm && ratItems.count() == 0))) return;

	if(ratArm && ratArm.rat == rat) return;

	bool ratBusy = false;
	ratItems.iterate([&ratBusy, rat](RatItem* ratItem){ if(ratItem->rat == rat) ratBusy = true; });
	if(ratBusy) return;

	if(lives == 0) return;

	ratArm = { rat, armGo->getPos() - rat->go->getPos(), rat->t };

	lives--;
	livesEl->setLives(lives);

	if(lives == 0){
		// TODO: lose audio
		return;
	}

	// TODO: lives-- audio

	state = Retracting;
}

std::vector<glm::vec2> DustyGame::DustyGame::randPoints(size_t count){
	static constexpr auto randPoint = [](){ return glm::vec2 { ((float) esp_random() / (float) UINT32_MAX), ((float) esp_random() / (float) UINT32_MAX) }; };

	if(count == 1) return { randPoint() };

	std::vector<glm::vec2> points;
	points.reserve(count * 4);
	for(int i = 0; i < count * 4; i++){
		points.push_back(randPoint());
	}

	while(points.size() > count){
		float maxDist = 10000;
		int maxA = 0, maxB = 1;

		for(int i = 0; i < points.size(); i++){
			for(int j = 0; j < points.size(); j++){
				if(j == i) continue;
				const auto dist = glm::distance(points[i], points[j]);

				if(dist < maxDist){
					maxDist = dist;
					maxA = i;
					maxB = j;
				}
			}
		}

		points.erase(points.begin() + ((esp_random() < UINT32_MAX/2) ? maxA : maxB));

		/* // Algo B:
		auto i = 1 + (esp_random() % (points.size() - 2));
		if(glm::distance(points[i], points[i-1]) < glm::distance(points[i], points[i+1])){
			points.erase(points.begin() + (i-1));
		}else{
			points.erase(points.begin() + (i+1));
		}*/
	}

	return points;
}
