#include "Planck.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "Util/stdafx.h"
#include "esp_random.h"

Planck::Planck::Planck(Sprite& canvas): Game(canvas, Games::Planck, "/Games/Planck", {
		{"/bat.raw", {}, true},
		{"/bat-bar.raw", {}, true},
		{"/bat-line.raw", {}, true},
		{"/boost.raw", {}, true},
		{"/car.raw", {}, true},
		{"/cones.raw", {}, true},
		{"/life.raw", {}, true},
		{"/lr.raw", {}, true},
		{"/lw.raw", {}, true},
		{"/hole.raw", {}, true},
		{"/pickup1.raw", {}, true},
		{"/pickup2.raw", {}, true},
		{"/rail.raw", {}, true},
		{"/ramp.raw", {}, true},
		{"/road.raw", {}, true},
		{"/rr.raw", {}, true},
		{"/rw.raw", {}, true},
		{"/trash.raw", {}, true},
		RES_HEART,
		RES_GOBLET
}){

}

void Planck::Planck::onLoad(){
	Game::onLoad();

	batteryBar = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bat-bar.raw"), PixelDim{ 71, 5 }),
			nullptr
	);
	batteryBar->setPos(2.0f, 2.0f);
	batteryBar->getRenderComponent()->setLayer(5);
	addObject(batteryBar);

	batteryLine = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bat-line.raw"), PixelDim{ 60, 1 }),
			nullptr
	);
	batteryLine->setPos(11.0f, 4.0f);
	batteryLine->getRenderComponent()->setLayer(6);
	addObject(batteryLine);

	setBattery(0.73f);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 10 });
	hearts->setLives(lives);
	addObject(hearts->getGO());

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 12 - 28, 2 });
	addObject(scoreDisplay->getGO());

	car = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/car.raw"), PixelDim{ 26, 30 }),
			std::make_unique<RectCC>(PixelDim{ 26, 30 })
	);
	car->setPos(51.0f, 80.0f);
	car->getRenderComponent()->setLayer(2);
	addObject(car);

	const float bottomY = 118.0f;

	for(int i = 0; i < 14; ++i){
		leftEdge[i] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(i % 2 == 0 ? "/lr.raw" : "/lw.raw"), PixelDim{ 10, 10 }),
				nullptr
		);

		rightEdge[i] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(i % 2 == 0 ? "/rr.raw" : "/rw.raw"), PixelDim{ 10, 10 }),
				nullptr
		);

		leftEdge[i]->setPos(0.0f, bottomY - i * 10.0f);
		rightEdge[i]->setPos(118.0f, bottomY - i * 10.0f);

		addObject(leftEdge[i]);
		addObject(rightEdge[i]);
	}

	for(int i = 0; i < VerticalTiles; ++i){
		if(i == VerticalTiles - 1){
			const uint8_t notObstacle = esp_random() % HorizontalTiles;

			road[i * HorizontalTiles] = createTile(notObstacle == 0 ? TileType::Road : (TileType)(esp_random() % 6));
			road[i * HorizontalTiles]->setPos(10.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles]);

			road[i * HorizontalTiles + 1] = createTile(notObstacle == 1 ? TileType::Road : (TileType)(esp_random() % 6));
			road[i * HorizontalTiles + 1]->setPos(46.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 1]);

			road[i * HorizontalTiles + 2] = createTile(notObstacle == 2 ? TileType::Road : (TileType)(esp_random() % 6));
			road[i * HorizontalTiles + 2]->setPos(82.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 2]);
		}else{
			road[i * HorizontalTiles] = createTile(TileType::Road);
			road[i * HorizontalTiles]->setPos(10.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles]);

			road[i * HorizontalTiles + 1] = createTile(TileType::Road);
			road[i * HorizontalTiles + 1]->setPos(46.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 1]);

			road[i * HorizontalTiles + 2] = createTile(TileType::Road);
			road[i * HorizontalTiles + 2]->setPos(82.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 2]);
		}
	}
}

void Planck::Planck::onLoop(float deltaTime){
	Game::onLoop(deltaTime);

	if(acceleration >= 1.0f && speed < speedLimits.y){
		setBattery(battery - 0.25f * deltaTime);
	}

	if(battery <= 0.0f && acceleration >= 1.0f){
		acceleration -= 1.0f;
	}

	if(lastBoost != 0 && millis() - lastBoost >= BoostDuration){
		acceleration -= 0.75f;
		lastBoost = 0;
	}

	if(lastAir != 0 && millis() - lastAir >= AirDuration){
		lastAir = 0;
	}

	speed += acceleration * 100.0f * deltaTime;
	speed = glm::clamp(speed, speedLimits.x, speedLimits.y);

	const glm::vec2 deltaPos = glm::vec2{ 0.0f, speed * deltaTime };

	car->setPos(car->getPos() + glm::vec2{ direction, 0.0f } * HorizontalSpeed * glm::pow((1.0f - map(speed, speedLimits.x, speedLimits.y, 0.0f, 0.25f)), 2.0f) * deltaTime);

	if(car->getPos().x < 10.0f){
		car->setPosX(10.0f);
	}

	if(car->getPos().x > 118.0f - 26.0f){
		car->setPosX(118.0f - 26.0f);
	}

	for(int i = 0; i < HorizontalTiles * VerticalTiles; ++i){
		road[i]->setPos(road[i]->getPos() + deltaPos);
	}

	std::vector<GameObjPtr> pickupsToRemove;
	for(const GameObjPtr& pickup : pickups){
		if(pickup){
			pickup->setPos(pickup->getPos() + deltaPos);
		}

		if(pickup->getPos().y > 128.0f){
			pickupsToRemove.emplace_back(pickup);
		}
	}

	for(const GameObjPtr& pickup : pickupsToRemove){
		pickups.erase(pickup);
		removeObject(pickup);
	}

	if(road[rowToGenerate * HorizontalTiles]->getPos().y >= 128.0f){
		generateRoad();
	}

	for(int i = 0; i < 14; ++i){
		leftEdge[i]->setPos(leftEdge[i]->getPos() + deltaPos);
		rightEdge[i]->setPos(rightEdge[i]->getPos() + deltaPos);

		if(leftEdge[i]->getPos().y >= 128.0f){
			leftEdge[i]->setPosY(leftEdge[i]->getPos().y - 140.0f);
		}

		if(rightEdge[i]->getPos().y >= 128.0f){
			rightEdge[i]->setPosY(rightEdge[i]->getPos().y - 140.0f);
		}
	}

	if(lives == 0){
		exit();
		return;
	}
}

void Planck::Planck::handleInput(const Input::Data& data){
	Game::handleInput(data);

	if((data.btn == Input::Button::Right && data.action == Input::Data::Press) || (data.btn == Input::Button::Left && data.action == Input::Data::Release)){
		direction += 1.0f;
	}

	if((data.btn == Input::Button::Left && data.action == Input::Data::Press) || (data.btn == Input::Button::Right && data.action == Input::Data::Release)){
		direction -= 1.0f;
	}

	if((data.btn == Input::Button::Up && data.action == Input::Data::Press && battery > 0.0f) || (data.btn == Input::Button::Down && data.action == Input::Data::Release)){
		acceleration += 1.0f;
	}

	if((data.btn == Input::Button::Down && data.action == Input::Data::Press) || (data.btn == Input::Button::Up && data.action == Input::Data::Release && battery > 0.0f)){
		acceleration -= 1.0f;
	}

	direction = std::clamp(direction, -1.0f, 1.0f);
}

void Planck::Planck::generateRoad(){
	removeObject(road[rowToGenerate * HorizontalTiles]);
	removeObject(road[rowToGenerate * HorizontalTiles + 1]);
	removeObject(road[rowToGenerate * HorizontalTiles + 2]);

	if(sinceGenerated < 2){
		road[rowToGenerate * HorizontalTiles] = createTile(TileType::Road);
		road[rowToGenerate * HorizontalTiles]->setPos(10.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles]);

		road[rowToGenerate * HorizontalTiles + 1] = createTile(TileType::Road);
		road[rowToGenerate * HorizontalTiles + 1]->setPos(46.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 1]);

		road[rowToGenerate * HorizontalTiles + 2] = createTile(TileType::Road);
		road[rowToGenerate * HorizontalTiles + 2]->setPos(82.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 2]);

		const uint8_t pickupLocation = esp_random() % HorizontalTiles;

		const float random = (1.0f * esp_random()) / INT_MAX;
		if(random <= 0.1f){
			GameObjPtr healthPickup = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/life.raw"), PixelDim{ 9, 7 }),
					std::make_unique<RectCC>(glm::vec2{9, 7})
			);

			healthPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{18.0f, 18.0f} - glm::vec2{4.5f, 3.5f});
			addObject(healthPickup);
			pickups.insert(healthPickup);
			healthPickup->getRenderComponent()->setLayer(1);

			collision.addPair(*car, *healthPickup, [this, healthPickup](){
				onHealthUp();
				pickups.erase(healthPickup);
				removeObject(healthPickup);
			});
		}else if(random <= 0.2f){
			GameObjPtr batteryPickup = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/bat.raw"), PixelDim{ 15, 17 }),
					std::make_unique<RectCC>(glm::vec2{15, 17})
			);

			batteryPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{18.0f, 18.0f} - glm::vec2{7.5f, 8.5f});
			addObject(batteryPickup);
			pickups.insert(batteryPickup);
			batteryPickup->getRenderComponent()->setLayer(1);

			collision.addPair(*car, *batteryPickup, [this, batteryPickup](){
				onBatteryUp();
				pickups.erase(batteryPickup);
				removeObject(batteryPickup);
			});
		}else if(random <= 0.7f){
			GameObjPtr coinPickup;

			if(esp_random() % 2 == 0){
				coinPickup = std::make_shared<GameObject>(
						std::make_unique<StaticRC>(getFile("/pickup1.raw"), PixelDim{ 16, 11 }),
						std::make_unique<RectCC>(glm::vec2{16, 11})
				);

				coinPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{18.0f, 18.0f} - glm::vec2{8.0f, 5.5f});
			}else{
				coinPickup = std::make_shared<GameObject>(
						std::make_unique<StaticRC>(getFile("/pickup2.raw"), PixelDim{ 11, 13 }),
						std::make_unique<RectCC>(glm::vec2{11, 13})
				);

				coinPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{18.0f, 18.0f} - glm::vec2{5.5f, 6.5f});
			}

			addObject(coinPickup);
			pickups.insert(coinPickup);
			coinPickup->getRenderComponent()->setLayer(1);

			collision.addPair(*car, *coinPickup, [this, coinPickup](){
				onPickup();
				pickups.erase(coinPickup);
				removeObject(coinPickup);
			});
		}

		++sinceGenerated;
	}else{
		const uint8_t notObstacle = esp_random() % HorizontalTiles;

		road[rowToGenerate * HorizontalTiles] = createTile(notObstacle == 0 ? TileType::Road : (TileType)(esp_random() % 6));
		road[rowToGenerate * HorizontalTiles]->setPos(10.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles]);

		road[rowToGenerate * HorizontalTiles + 1] = createTile(notObstacle == 1 ? TileType::Road : (TileType)(esp_random() % 6));
		road[rowToGenerate * HorizontalTiles + 1]->setPos(46.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 1]);

		road[rowToGenerate * HorizontalTiles + 2] = createTile(notObstacle == 2 ? TileType::Road : (TileType)(esp_random() % 6));
		road[rowToGenerate * HorizontalTiles + 2]->setPos(82.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 2]);

		sinceGenerated = 0;
	}

	rowToGenerate = (rowToGenerate + 1) % VerticalTiles;
}

bool Planck::Planck::onCollision(){
	if(lastAir != 0 || lives == 0){
		return false;
	}

	--lives;
	hearts->setLives(lives);

	return true;
}

void Planck::Planck::onBoost(){
	if(lastAir != 0){
		return;
	}

	if(lastBoost == 0){
		acceleration += 0.75f;
	}

	lastBoost = millis();
}

void Planck::Planck::onRamp(){
	lastAir = millis();
}

void Planck::Planck::onHealthUp(){
	if(lives >= 3 || lastAir != 0){
		return;
	}

	++lives;
	hearts->setLives(lives);
}

void Planck::Planck::onBatteryUp(){
	if(lastAir != 0){
		return;
	}

	setBattery(battery + 0.5f);
}

void Planck::Planck::onPickup(){
	if(lastAir != 0){
		return;
	}

	++score;
	scoreDisplay->setScore(score);
}

GameObjPtr Planck::Planck::createTile(Planck::Planck::TileType type){
	if(type == Planck::TileType::Road){
		return std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
				nullptr
		);
	}

	std::unique_ptr<RenderComponent> rc = std::make_unique<StaticRC>(getFile(Obstacles[(uint8_t) type]), PixelDim{ 36, 36 });
	std::unique_ptr<CollisionComponent> cc = nullptr;

	switch(type){
		case Planck::TileType::Cones:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{ {8, 26}, {8, 22}, {12, 11}, {23, 11}, {27, 22}, {27, 26} });
			break;
		}
		case Planck::TileType::Hole:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{ {5, 22}, {8, 14}, {12, 9}, {18, 9}, {28, 12}, {31, 15}, {31, 17}, {22, 26}, {8, 26} });
			break;
		}
		case Planck::TileType::Rail:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{ {6, 28}, {4, 12}, {6, 10}, {29, 10}, {31, 12}, {30, 28} });
			break;
		}
		case Planck::TileType::Ramp:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{ {4, 33}, {4, 1}, {31, 1}, {31, 33} });
			break;
		}
		case Planck::TileType::Trash:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{ {6, 23}, {4, 20}, {4, 13}, {6, 11}, {17, 11}, {32, 17}, {32, 21}, {29, 23} });
			break;
		}
		case Planck::TileType::Boost:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{ {4, 30}, {4, 11}, {17, 5}, {19, 5}, {32, 11}, {32, 30} });
			break;
		}
		default:{
			break;
		}
	}

	GameObjPtr obj = std::make_shared<GameObject>(
			std::move(rc),
			std::move(cc)
	);

	switch(type){
		case Planck::TileType::Cones:{
			collision.addPair(*car, *obj, [this, obj](){
				if(onCollision()){
					auto rc = (StaticRC*) obj->getRenderComponent().get();
					rc->setFile(getFile("/road.raw"));
					collision.removePair(*car, *obj);
				}
			});
			break;
		}
		case Planck::TileType::Hole:{
			collision.addPair(*car, *obj, [this, obj](){
				onCollision();
				collision.removePair(*car, *obj);
			});
			break;
		}
		case Planck::TileType::Rail:{
			collision.addPair(*car, *obj, [this, obj](){
				if(onCollision()){
					auto rc = (StaticRC*) obj->getRenderComponent().get();
					rc->setFile(getFile("/road.raw"));
					collision.removePair(*car, *obj);
				}
			});
			break;
		}
		case Planck::TileType::Ramp:{
			collision.addPair(*car, *obj, [this, obj](){
				if(car->getPos().y <= obj->getPos().y + 30){
					return;
				}

				onRamp();
			});
			break;
		}
		case Planck::TileType::Trash:{
			collision.addPair(*car, *obj, [this, obj](){
				if(onCollision()){
					auto rc = (StaticRC*) obj->getRenderComponent().get();
					rc->setFile(getFile("/road.raw"));
					collision.removePair(*car, *obj);
				}
			});
			break;
		}
		case Planck::TileType::Boost:{
			collision.addPair(*car, *obj, [this](){
				onBoost();
			});
			break;
		}
		default:{
			break;
		}
	}

	return obj;
}

void Planck::Planck::setBattery(float percent){
	battery = std::clamp(percent, 0.0f, 1.0f);

	StaticRC* rc = (StaticRC*)(batteryLine->getRenderComponent().get());
	rc->setFile(getFile("/bat-line.raw"), {battery * 60, 1});
}
