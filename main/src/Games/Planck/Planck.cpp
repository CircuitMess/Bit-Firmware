#include "Planck.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "Util/stdafx.h"
#include "esp_random.h"
#include "Util/Services.h"

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

void Planck::Planck::onStart(){
	const auto input = (Input*) Services.get(Service::Input);
	direction = 0;
	poweredAcceleration = 0;

	if(input->isPressed(Input::Button::Right) && !input->isPressed(Input::Button::Left)){
		direction = 1.0f;
	}

	if(input->isPressed(Input::Button::Left) && !input->isPressed(Input::Button::Right)){
		direction = -1.0f;
	}

	if(input->isPressed(Input::Button::A) && !input->isPressed(Input::Button::B)){
		poweredAcceleration = 1.0f;
	}

	if(input->isPressed(Input::Button::B) && !input->isPressed(Input::Button::A)){
		poweredAcceleration = -1.0f;
	}

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
			std::make_unique<SpriteRC>(PixelDim{ 26, 30 }),
			std::make_unique<RectCC>(PixelDim{ 24, 15 }, PixelDim{ 1, 0 })
	);

	carRC = std::static_pointer_cast<SpriteRC>(car->getRenderComponent());
	carRC->getSprite()->clear(TFT_TRANSPARENT);
	Display::drawFile(*carRC->getSprite(), getFile("/car.raw"), 0, 0, 26, 30);
	car->setPos(51.0f, CarYPos);
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

			bool newHolePending[3] = { 0 };

			float random = (float) esp_random() / (float) UINT32_MAX;

			uint8_t numObstacles = 0;
			if(random < 0.75){
				numObstacles = 1;
			}else{
				numObstacles = 2;
			}

			uint8_t obstaclesLocation[2] = { 0 };
			uint8_t obstacleColumn = esp_random() % HorizontalTiles;
			for(int x = 0; x < numObstacles; ++x){
				for(; obstacleColumn < HorizontalTiles; obstacleColumn = (obstacleColumn + 1) % HorizontalTiles){
					if(holePending[obstacleColumn]){
						continue;
					}else if(!holePending[obstacleColumn]){
						break;
					}
				}
				obstaclesLocation[x] = obstacleColumn;
				obstacleColumn = (obstacleColumn + 1) % HorizontalTiles;
			}

			//new obstacles
			TileType firstType;
			for(uint8_t x = 0; x < numObstacles; x++){
				uint8_t location = obstaclesLocation[x];

				random = (float) esp_random() / (float) UINT32_MAX;
				TileType type = Road;

				if(random < 0.9){
					type = (TileType) (esp_random() % 3);
				}else if(random < 0.95){
					type = Ramp;
				}else{
					type = Boost;
				}

				if(x == 0){
					firstType = type;
				}

				if(x == 1 && type == firstType){
					type = (TileType) ((type + 1) % 5);
				}

				road[i * HorizontalTiles + location] = createTile(type);

				if(type == Ramp){
					newHolePending[location] = true;
				}

				road[i * HorizontalTiles + location]->setPos(10.0f + 36 * location, 96.0f - i * 36.0f);
				addObject(road[i * HorizontalTiles + location]);
			}

			//rest is road
			for(uint8_t j = 0; j < HorizontalTiles; j++){
				if(!holePending[j] && (obstaclesLocation[0] != j || numObstacles < 1) && (obstaclesLocation[1] != j || numObstacles < 2)){
					road[i * HorizontalTiles + j] = createTile(TileType::Road);
					road[i * HorizontalTiles + j]->setPos(10.0f + 36 * j, 96.0f - i * 36.0f);
					addObject(road[i * HorizontalTiles + j]);
				}
			}

			for(uint8_t x = 0; x < 3; x++){
				holePending[x] = newHolePending[x];
			}
		}else{
			for(uint8_t j = 0; j < HorizontalTiles; j++){
				road[i * HorizontalTiles + j] = createTile(TileType::Road);
				road[i * HorizontalTiles + j]->setPos(10.0f + 36 * j, 96.0f - i * 36.0f);
				addObject(road[i * HorizontalTiles + j]);
			}
		}
	}
}

void Planck::Planck::onLoop(float deltaTime){
	Game::onLoop(deltaTime);
	const auto input = (Input*) Services.get(Service::Input);

	if(boosting){
		boostProgress += deltaTime;
		if(boostProgress >= BoostDuration){
			boosting = false;
			boostAcceleration = 0;
		}
	}

	if(lastAir != 0){
		if(millis() - lastAir >= AirDuration){
			lastAir = 0;
			carRC->setScale({ 1.0f, 1.0f });

			if(input->isPressed(Input::Button::Right) && !input->isPressed(Input::Button::Left)){
				direction = 1.0f;
			}

			if(input->isPressed(Input::Button::Left) && !input->isPressed(Input::Button::Right)){
				direction = -1.0f;
			}

			if(input->isPressed(Input::Button::A) && !input->isPressed(Input::Button::B)){
				poweredAcceleration = 1.0f;
			}

			if(input->isPressed(Input::Button::B) && !input->isPressed(Input::Button::A)){
				poweredAcceleration = -1.0f;
			}
		}else{
			const auto x = millis() - lastAir;
			const auto carZoom = -(JumpMaxZoom - 1.0) / 2.0 * cos(2.0 * M_PI / (float) AirDuration * x) + 1 + (JumpMaxZoom - 1.0) / 2.0;

			carRC->setScale({ carZoom, carZoom });
		}
	}

	if(input->isPressed(Input::Button::A) && lastAir == 0){
		setBattery(battery - BatteryDischargeRate * deltaTime);
	}

	if(battery <= 0.0f && poweredAcceleration >= 1.0f){
		poweredAcceleration -= 1.0f;

		//no energy left
		audio.play({{ 100, 100, 100 },
					{ 0,   0,   75 },
					{ 100, 100, 100 },
				   });
	}

	auto a = poweredAcceleration * GasBrakeAcceleration + boostAcceleration;
	if(lastAir == 0){
		a += PassiveDeceleration; //no passive deceleration mid-air
	}
	speed += a * deltaTime;
	speed = glm::clamp(speed, speedLimits.x, speedLimits.y);

	const glm::vec2 deltaPos = glm::vec2{ 0.0f, speed * deltaTime };

	car->setPos(car->getPos() +
				glm::vec2{ direction, 0.0f } * HorizontalSpeed * glm::pow((1.0f - map(speed, speedLimits.x, speedLimits.y, 0.0f, 0.25f)), 2.0f) * deltaTime);

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
	for(const GameObjPtr& pickup: pickups){
		if(pickup){
			pickup->setPos(pickup->getPos() + deltaPos);
		}

		if(pickup->getPos().y > 128.0f){
			pickupsToRemove.emplace_back(pickup);
		}
	}

	for(const GameObjPtr& pickup: pickupsToRemove){
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

	if(invincible){
		invincibilityTime += deltaTime;
		if((int) (invincibilityTime / InvincibilityBlinkDuration) % 2 == 0){
			car->getRenderComponent()->setVisible(false);
		}else{
			car->getRenderComponent()->setVisible(true);
		}

		if(invincibilityTime >= InvincibilityDuration){
			invincibilityTime = 0;
			invincible = false;
			car->getRenderComponent()->setVisible(true);
		}
	}

	if(lives == 0 || (speed == 0 && battery == 0)){
		audio.play({{ 100, 500, 200 },
					{ 500, 100, 200 },
					{ 0,   0,   50 },
					{ 100, 350, 200 },
					{ 350, 100, 200 },
					{ 0,   0,   50 },
					{ 100, 100, 150 },
					{ 0,   0,   100 },
					{ 100, 100, 350 },
				   });
		delayMillis(2000);
		exit();
		return;
	}
}

void Planck::Planck::handleInput(const Input::Data& data){
	Game::handleInput(data);

	if(lastAir != 0) return; //no handling mid-air

	if((data.btn == Input::Button::Right && data.action == Input::Data::Press) || (data.btn == Input::Button::Left && data.action == Input::Data::Release)){
		direction += 1.0f;
	}

	if((data.btn == Input::Button::Left && data.action == Input::Data::Press) || (data.btn == Input::Button::Right && data.action == Input::Data::Release)){
		direction -= 1.0f;
	}

	if(data.btn == Input::Button::A && data.action == Input::Data::Press){
		if(battery > 0.0f){
			poweredAcceleration += 1.0f;
		}else{
			//no energy left
			audio.play({{ 100, 100, 100 },
						{ 0,   0,   75 },
						{ 100, 100, 100 },
					   });
		}

	}
	if(data.btn == Input::Button::B && data.action == Input::Data::Release){
		poweredAcceleration += 1.0f;
	}

	if((data.btn == Input::Button::B && data.action == Input::Data::Press) ||
	   (data.btn == Input::Button::A && data.action == Input::Data::Release && battery > 0.0f)){
		poweredAcceleration -= 1.0f;
	}

	direction = std::clamp(direction, -1.0f, 1.0f);
}

void Planck::Planck::generateRoad(){
	removeObject(road[rowToGenerate * HorizontalTiles]);
	removeObject(road[rowToGenerate * HorizontalTiles + 1]);
	removeObject(road[rowToGenerate * HorizontalTiles + 2]);

	int8_t prevRow = (int8_t) rowToGenerate - 1;
	if(prevRow < 0) prevRow = VerticalTiles - 1;


	bool newHolePending[3] = { 0 };

	uint8_t nonHoleLocations = 0;
	for(uint8_t i = 0; i < HorizontalTiles; i++){
		if(!holePending[i]){
			nonHoleLocations++;
		}
	}


	if(sinceGenerated < nextObstacleDistance){
		for(uint8_t i = 0; i < HorizontalTiles; i++){
			if(holePending[i]){
				road[rowToGenerate * HorizontalTiles + i] = createTile(TileType::Hole);
			}else{
				road[rowToGenerate * HorizontalTiles + i] = createTile(TileType::Road);
			}
			road[rowToGenerate * HorizontalTiles + i]->setPos(10.0f + 36.0f * i, road[prevRow * HorizontalTiles]->getPos().y - 36.0f);
			addObject(road[rowToGenerate * HorizontalTiles + i]);
		}


		if(nonHoleLocations > 0){
			uint8_t pickupLocation = esp_random() % nonHoleLocations;

			for(uint8_t i = 0; i < HorizontalTiles; i++){
				if(holePending[i] && i == pickupLocation){
					pickupLocation++;
				}else if(!holePending[i] && i == pickupLocation){
					break;
				}
			}


			const float random = (float) esp_random() / (float) UINT32_MAX;
			if(random <= 0.05f){
				GameObjPtr healthPickup = std::make_shared<GameObject>(
						std::make_unique<StaticRC>(getFile("/life.raw"), PixelDim{ 9, 7 }),
						std::make_unique<RectCC>(glm::vec2{ 9, 7 })
				);

				healthPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{ 18.0f, 18.0f } - glm::vec2{ 4.5f, 3.5f });
				addObject(healthPickup);
				pickups.insert(healthPickup);
				healthPickup->getRenderComponent()->setLayer(1);

				collision.addPair(*car, *healthPickup, [this, healthPickup](){
					onHealthUp();
					pickups.erase(healthPickup);
					removeObject(healthPickup);
				});
			}else if(random <= 0.1){
				GameObjPtr batteryPickup = std::make_shared<GameObject>(
						std::make_unique<StaticRC>(getFile("/bat.raw"), PixelDim{ 15, 17 }),
						std::make_unique<RectCC>(glm::vec2{ 15, 17 })
				);

				batteryPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{ 18.0f, 18.0f } - glm::vec2{ 7.5f, 8.5f });
				addObject(batteryPickup);
				pickups.insert(batteryPickup);
				batteryPickup->getRenderComponent()->setLayer(1);

				collision.addPair(*car, *batteryPickup, [this, batteryPickup](){
					onBatteryUp();
					pickups.erase(batteryPickup);
					removeObject(batteryPickup);
				});
			}else if(random <= 0.2f){
				GameObjPtr coinPickup;

				if(esp_random() % 2 == 0){
					coinPickup = std::make_shared<GameObject>(
							std::make_unique<StaticRC>(getFile("/pickup1.raw"), PixelDim{ 16, 11 }),
							std::make_unique<RectCC>(glm::vec2{ 16, 11 })
					);

					coinPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{ 18.0f, 18.0f } - glm::vec2{ 8.0f, 5.5f });
				}else{
					coinPickup = std::make_shared<GameObject>(
							std::make_unique<StaticRC>(getFile("/pickup2.raw"), PixelDim{ 11, 13 }),
							std::make_unique<RectCC>(glm::vec2{ 11, 13 })
					);

					coinPickup->setPos(road[rowToGenerate * HorizontalTiles + pickupLocation]->getPos() + glm::vec2{ 18.0f, 18.0f } - glm::vec2{ 5.5f, 6.5f });
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
		}

		++sinceGenerated;
	}else{

		float random = (float) esp_random() / (float) UINT32_MAX;

		uint8_t numObstacles = 0;
		if(nonHoleLocations == 1){
			if(random < 0.75){
				numObstacles = 0;
			}else{
				numObstacles = 1;
			}
		}else if(nonHoleLocations == 2){
			if(random < 0.1){
				numObstacles = 2; // 2 obstacles and one ramp which is the only route
			}else if(random < 0.5){
				numObstacles = 1;
			}else{
				numObstacles = 0;
			}
		}else if(nonHoleLocations == 3){
			if(random < 0.75){
				numObstacles = 1;
			}else{
				numObstacles = 2;
			}
		}

		uint8_t obstaclesLocation[2] = { 0 };
		uint8_t obstacleColumn = esp_random() % HorizontalTiles;
		for(int x = 0; x < numObstacles; ++x){
			for(; obstacleColumn < HorizontalTiles; obstacleColumn = (obstacleColumn + 1) % HorizontalTiles){
				if(holePending[obstacleColumn]){
					continue;
				}else if(!holePending[obstacleColumn]){
					break;
				}
			}
			obstaclesLocation[x] = obstacleColumn;
			obstacleColumn = (obstacleColumn + 1) % HorizontalTiles;
		}

		//pending holes
		for(uint8_t i = 0; i < HorizontalTiles; i++){
			if(holePending[i]){
				road[rowToGenerate * HorizontalTiles + i] = createTile(TileType::Hole);
				const float y = road[prevRow * HorizontalTiles]->getPos().y - 36.0f;
				road[rowToGenerate * HorizontalTiles + i]->setPos(10.0f + 36.0f * i, y);
				addObject(road[rowToGenerate * HorizontalTiles + i]);
			}
		}

		//new obstacles
		TileType firstType;
		for(uint8_t x = 0; x < numObstacles; x++){
			uint8_t location = obstaclesLocation[x];

			random = (float) esp_random() / (float) UINT32_MAX;
			TileType type = Road;

			if(random < 0.9){
				type = (TileType) (esp_random() % 3);
			}else if(random < 0.95){
				type = Ramp;
			}else{
				type = Boost;
			}

			if(x == 0){
				firstType = type;
			}

			if(x == 1 && type == firstType){
				type = (TileType) ((type + 1) % 5);
			}

			road[rowToGenerate * HorizontalTiles + location] = createTile(type);

			if(type == Ramp){
				newHolePending[location] = true;
			}

			road[rowToGenerate * HorizontalTiles + location]->setPos(10.0f + 36.0f * location, road[prevRow * HorizontalTiles]->getPos().y - 36.0f);
			addObject(road[rowToGenerate * HorizontalTiles + location]);
		}

		//rest is road
		for(uint8_t i = 0; i < HorizontalTiles; i++){
			if(!holePending[i] && (obstaclesLocation[0] != i || numObstacles < 1) && (obstaclesLocation[1] != i || numObstacles < 2)){
				road[rowToGenerate * HorizontalTiles + i] = createTile(TileType::Road);
				road[rowToGenerate * HorizontalTiles + i]->setPos(10.0f + 36.0f * i, road[prevRow * HorizontalTiles]->getPos().y - 36.0f);
				addObject(road[rowToGenerate * HorizontalTiles + i]);
			}
		}

		sinceGenerated = 0;
		nextObstacleDistance = esp_random() % (MaxNextObstacleDistance - MinNextObstacleDistance + 1) + MinNextObstacleDistance;
	}


	for(uint8_t i = 0; i < 3; i++){
		holePending[i] = newHolePending[i];
	}

	rowToGenerate = (rowToGenerate + 1) % VerticalTiles;
}

bool Planck::Planck::onCollision(){
	if(lastAir != 0 || lives == 0){
		return false;
	}

	if(!invincible){
		//taken damage
		audio.play({{ 500, 180, 250 },
					{ 0,   0,   50 },
					{ 100, 100, 100 },
					{ 0,   0,   75 },
					{ 100, 100, 100 }
				   });

		--lives;
		hearts->setLives(lives);

		invincible = true;
	}

	return true;
}

void Planck::Planck::onBoost(){
	if(lastAir != 0){
		return;
	}

	//boosting
	audio.play({{ 300, 800, 750 }});
	boostAcceleration = BoostAccelerationRate;

	boosting = true;
	boostProgress = 0;
}

void Planck::Planck::onRamp(){
	if(speed < MinimumRampSpeed){
		return;
	}

	if(lastAir != 0){
		return;
	}

	//jumping
	audio.play({{ 400, 750, 650 },
				{ 750, 400, 650 },
				{ 0, 0, 50 },
				{ 80, 80, 75 }
			   });

	lastAir = millis();

	direction = 0;
	poweredAcceleration = 0;
}

void Planck::Planck::onHealthUp(){
	if(lastAir != 0){
		return;
	}

	audio.play({{ 250, 500, 150 },
				{ 0,   0,   75 },
				{ 350, 600, 150 },
				{ 0,   0,   75 },
				{ 450, 700, 150 },
			   });

	if(lives >= 3){
		return;
	}

	++lives;
	hearts->setLives(lives);
}

void Planck::Planck::onBatteryUp(){
	if(lastAir != 0){
		return;
	}

	audio.play({{ 300, 750, 200 },
				{ 0,   0,   75 },
				{ 400, 850, 200 }
			   });

	setBattery(battery + 0.5f);

	batteries++;
	setAchiIfBigger(Achievement::Planck_juice, batteries);
}

void Planck::Planck::onPickup(){
	if(lastAir != 0){
		return;
	}

	audio.play({{ 600, 600, 100 },
				{ 0,   0,   75 },
				{ 800, 800, 100 }});
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
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{
					{ 8,  26 },
					{ 8,  22 },
					{ 12, 11 },
					{ 23, 11 },
					{ 27, 22 },
					{ 27, 26 }});
			break;
		}
		case Planck::TileType::Rail:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{
					{ 6,  28 },
					{ 4,  12 },
					{ 6,  10 },
					{ 29, 10 },
					{ 31, 12 },
					{ 30, 28 }});
			break;
		}
		case Planck::TileType::Ramp:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{
					{ 4,  33 },
					{ 4,  1 },
					{ 31, 1 },
					{ 31, 33 }});
			break;
		}
		case Planck::TileType::Trash:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{
					{ 6,  23 },
					{ 4,  20 },
					{ 4,  13 },
					{ 6,  11 },
					{ 17, 11 },
					{ 32, 17 },
					{ 32, 21 },
					{ 29, 23 }});
			break;
		}
		case Planck::TileType::Boost:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{
					{ 4,  30 },
					{ 4,  11 },
					{ 17, 5 },
					{ 19, 5 },
					{ 32, 11 },
					{ 32, 30 }});
			break;
		}
		case Planck::TileType::Hole:{
			cc = std::make_unique<PolygonCC>(std::initializer_list<glm::vec2>{
					{ 5,  22 },
					{ 8,  14 },
					{ 12, 9 },
					{ 18, 9 },
					{ 28, 12 },
					{ 31, 15 },
					{ 31, 17 },
					{ 22, 26 },
					{ 8,  26 }});
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
		case Planck::TileType::Hole:{
			collision.addPair(*car, *obj, [this, obj](){
				onCollision();
				collision.removePair(*car, *obj);
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

	StaticRC* rc = (StaticRC*) (batteryLine->getRenderComponent().get());
	rc->setFile(getFile("/bat-line.raw"), { battery * 60, 1 });
}
