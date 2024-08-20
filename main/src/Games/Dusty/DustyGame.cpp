#include "DustyGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"
#include <gtx/rotate_vector.hpp>
#include <esp_random.h>

DustyGame::DustyGame::DustyGame(Sprite& canvas) : Game(canvas, Games::Dusty, "/Games/Dusty", {
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
			std::make_unique<RectCC>(PixelDim { 5, 7 }, PixelDim { 0, 7 })
	);
	armGo->setPos(ArmPos);
	armGo->getRenderComponent()->setLayer(20);
	addObject(armGo);

	const auto wc = [this](){ wallCollision(); };
	collision.wallLeft(*armGo, wc);
	collision.wallRight(*armGo, wc);
	collision.wallBot(*armGo, wc);
}

void DustyGame::DustyGame::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;
	if(data.btn != Input::A) return;

	if(state == Aiming){
		shoot();
	}
}

void DustyGame::DustyGame::onRender(Sprite& canvas){
	if(state != Shooting && state != Retracting) return;

	glm::vec2 armLinePos = glm::vec2 { 2, 14 };
	glm::rotate(armLinePos, shootAngl);

	canvas.drawLine(ArmPos.x + 2, ArmPos.y + 14, std::round(armGo->getPos().x + armLinePos.x), std::round(armGo->getPos().y + armLinePos.y), TFT_BLACK);
}

void DustyGame::DustyGame::onStart(){
	spawnItems();
}

void DustyGame::DustyGame::onLoop(float deltaTime){
	if(state == Aiming){
		updateAim(deltaTime);
	}else if(state == Shooting){
		updateShoot(deltaTime);
	}else if(state == Retracting){
		updateRetract(deltaTime);
	}

	updateSpawn(deltaTime);
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
			// TODO: item caught sound

			removeObject(caught.item->go);
			items.rem(caught.item);
			delete caught.item;
			caught = {};

			if(items.count() == 0){
				level++;
				spawnItems();
			}
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

	const auto points = randPoints(MaxItems);
	for(int i = 0; i < MaxItems; i++){
		const int id = esp_random() % ItemCount;
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

void DustyGame::DustyGame::itemCollision(Item* item){
	if(state != Shooting) return;
	state = Retracting;

	caught = { item, item->go->getPos() - armGo->getPos() };
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
