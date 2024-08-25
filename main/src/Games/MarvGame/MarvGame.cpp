#include "MarvGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "Services/RobotManager.h"
#include "Util/Services.h"
#include <esp_random.h>

MarvGame::MarvGame::MarvGame(Sprite& canvas) : Game(canvas, Games::Marv, "/Games/Marv", {
		{ "/bg.raw", {}, true },
		{ "/podium.raw", {}, false },
		{ "/fence.raw", {}, true },

		{ "/hydrant.raw", {}, true },
		{ "/banana.raw", {}, true },
		{ "/manhole.raw", {}, true },
		{ "/pidgeonWalk.raw", {}, true },
		{ "/weeds.raw", {}, true },

		{ "/newspaper.raw", {}, true },
		{ "/tp.raw", {}, true },
		{ "/pidgeonFly.raw", {}, true },
		{ "/umbrella.raw", {}, true },

		{ "/duck.gif", {}, false },
		{ "/ducking.gif", {}, false },
		{ "/unducking.gif", {}, false },
		{ "/jump.gif", {}, false },
		{ "/walkDead.gif", {}, false },
		{ "/win.gif", {}, false },
		{ "/walk.gif", {}, true },
		RES_HEART
}){
	robot = std::make_shared<RoboCtrl::Marv>();
	setRobot(robot);
}

void MarvGame::MarvGame::onLoad(){
	setupObstacles();
	leftWallObject = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 10, 128 })
	);
	leftWallObject->setPos({ -55, 0 });
	addObject(leftWallObject);


	///Tiles
	tileManager = std::make_unique<TileManager>(movingTiles);
	tileManager->addFile(getFile("/fence.raw"));
	tileManager->create(); // creates GameObjects into movingTiles



	leftWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 1, 128 })
	);
	leftWall->setPos({ -tileDim * tilesPerArray, 0 });
	addObject(leftWall);


	for(int i = 0; i < movingTiles.size(); i++){
		auto obj = movingTiles[i];
		addObject(obj);
		collision.addPair(*leftWall, *obj, [this, i](){ tileManager->reset(i); });
	}

	///Background
	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-2);
	addObject(bg);
	///Duck
	std::initializer_list<glm::vec2> points = { { 26, 63 },
												{ 16, 35 },
												{ 26, 9 },
												{ 37, 9 },
												{ 37, 63 } };
	auto duckGoCc = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<PolygonCC>(points, glm::vec2{ 31, 36 })
	);

	addObject(duckGoCc);
	duckGoCc->setPos({ 5, 50 });

	auto duckGoRc = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/walk.gif")),
			nullptr
	);
	addObject(duckGoRc);
	duckGoRc->getRenderComponent()->setLayer(10);
	duckGoRc->setPos({ 5, 50 });

	duck = std::make_unique<Player>(duckGoRc, duckGoCc, this);
	duck->setFiles(getFile("/walk.gif"),
				   getFile("/walkDead.gif"),
				   getFile("/jump.gif"),
				   getFile("/ducking.gif"),
				   getFile("/duck.gif"),
				   getFile("/unducking.gif"),
				   getFile("/win.gif"));
	///Score
	std::string scoreText = "Score:" + std::to_string(score) + "/" + std::to_string(scoreMax);
	auto scoreObj = std::make_shared<GameObject>(
			std::make_unique<TextRC>(scoreText, TextStyle{ &Font0, TFT_WHITE, 1, TR_DATUM }),
			nullptr);
	scoreObj->setPos({ 128 - 2, 2 });
	scoreTextRC = std::static_pointer_cast<TextRC>(scoreObj->getRenderComponent());
	addObject(scoreObj);
	///Hearts
	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());

	robot->setSpeed(StartingLightsSpeed);
}

void MarvGame::MarvGame::onLoop(float deltaTime){
	if(endPauseState != Running){
		endPauseCounter += deltaTime;
		if((endPauseState == Win && endPauseCounter >= WinPause) || (endPauseState == Lose && endPauseCounter >= DeathPause)){
			exit();
		}
		return;
	}

	duck->update(deltaTime);
	for(auto obj : movingObjects){
		float x = obj->getPos().x - deltaTime * speed;
		float y = obj->getPos().y;
		obj->setPos({ x, y });
	}
	for(auto obj : movingTiles){
		float x = obj->getPos().x - deltaTime * speed * TilesSpeedMultiplier;
		float y = obj->getPos().y;
		obj->setPos({ x, y });
	}

	if(isDone){
		if(speed == 0) return;
		if(podium->getPos().x <= 58){
			auto diff = 58 - podium->getPos().x;

			for(auto obj : movingObjects){
				float x = obj->getPos().x + diff;
				float y = obj->getPos().y;
				obj->setPos({ x, y });
			}

			for(auto obj : movingTiles){
				float x = obj->getPos().x + diff;
				float y = obj->getPos().y;
				obj->setPos({ x, y });
			}

			duck->win();
			speed = 0;

			if(life == 3){
				addAchi(Achievement::Marv_life, 1);
				addAchi(Achievement::Marv_5, 1);
			}
		}
	}else{
		spawnValue += deltaTime;
		if(spawnValue >= spawnRate){
			spawnValue -= spawnRate;
			spawn();
		}
	}
}

void MarvGame::MarvGame::onStop(){
	duck->duckReleased();
}

uint32_t MarvGame::MarvGame::getXP() const{
	float success = (float)(score)/(float)(scoreMax);
	return success * 250.0f;
}

void MarvGame::MarvGame::setupObstacles(){
	obstacleOver.push_back({ getFile("/hydrant.raw"), { 17, 33 }, {} });
	obstacleOver.push_back({ getFile("/banana.raw"), { 22, 23 }, {} });
	obstacleOver.push_back({ getFile("/manhole.raw"), { 38, 20 }, {} });
	obstacleOver.push_back({ getFile("/pidgeonWalk.raw"), { 23, 23 }, { { 0, 20 }, { 0, 1 }, { 7, 1 }, { 22, 13 }, { 22, 20 } } });
	obstacleOver.push_back({ getFile("/weeds.raw"), { 29, 43 }, { { 0, 43 }, { 0, 17 }, { 9, 0 }, { 15, 0 }, { 28, 10 }, { 28, 42 } } });

	obstacleUnder.push_back({ getFile("/newspaper.raw"), { 31, 24 }, { { 14, 23 }, { 0, 17 }, { 13, 0 }, { 30, 3 } } });
	obstacleUnder.push_back({ getFile("/tp.raw"), { 20, 16 }, { { 4, 15 }, { 0, 11 }, { 7, 0 }, { 19, 8 }, { 13, 15 } } });
	obstacleUnder.push_back({ getFile("/pidgeonFly.raw"), { 41, 11 }, { { 0, 5 }, { 16, 0 }, { 22, 0 }, { 40, 5 }, { 22, 10 }, { 16, 10 } } });
	obstacleUnder.push_back({ getFile("/umbrella.raw"), { 26, 30 }, { { 11, 29 }, { 0, 23 }, { 10, 1 }, { 20, 0 }, { 25, 23 } } });
}

void MarvGame::MarvGame::spawn(){
	if(speed < speedMax){
		speed += speedIncrement;
		spawnRate -= spawnIncrement;
	}

	if(score >= scoreMax - 1){
		isDone = true;
		podium = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("/podium.raw"), PixelDim{ 66, 32 }),
				nullptr
		);
		addObject(podium);
		podium->getRenderComponent()->setLayer(0);
		podium->setPos({ 128, topY - 20 });
		movingObjects.push_back(podium);
		return;
	}

	if(obstacleIndex.empty()){
		for(int i = 0; i < obstacleOver.size() + obstacleUnder.size(); i++){
			obstacleIndex.insert(i);
		}
	}

	auto item = obstacleIndex.begin();
	std::advance(item, esp_random() % obstacleIndex.size());

	uint8_t in = *item;
	Obstacle obstacle;
	int posY;
	if(in >= obstacleOver.size()){
		obstacle = obstacleUnder[in - obstacleOver.size()];
		posY = topY - 40 - obstacle.dim.y;
	}else{
		obstacle = obstacleOver[in];
		posY = topY - obstacle.dim.y / 2;
	}
	obstacleIndex.erase(item);

	std::shared_ptr<GameObject> gObj;
	if(obstacle.points.size() >= 3){
		gObj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(obstacle.file, obstacle.dim),
				std::make_unique<PolygonCC>(obstacle.points)
		);
	}else{
		gObj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(obstacle.file, obstacle.dim),
				std::make_unique<RectCC>(obstacle.dim)
		);
	}
	addObject(gObj);
	gObj->setPos({ 128, posY });
	gObj->getRenderComponent()->setLayer(1);

	collision.addPair(*gObj, *duck->getGameObjectCc(), [this](){
		if(duck->invincible) return;
		duckHit();
	});
	movingObjects.push_back(gObj);
	collision.addPair(*leftWallObject, *gObj, [this](){
		removeObject(movingObjects[0]);
		movingObjects.erase(movingObjects.begin());
		scoreUp();
	});
}

void MarvGame::MarvGame::duckHit(){
//	RGB.blinkTwice(Pixel::Red);

	life--;
	hearts->setLives(life);
	if(life == 0){
		speed = 0.0f;
		spawnRate = 10000.0f;
		duck->death();
		resetAchi(Achievement::Marv_5);
		audio.play({ { 300, 400, 100 },
					 { 400, 300, 100 },
					 { 200, 300, 100 },
					 { 300, 200, 100 },
					 { 80,  80,  300 } });
		return;
	}
	duck->invincible = true;
	audio.play({ { 100, 100, 50 },
				 { 0,   0,   25 },
				 { 300, 100, 100 } });
}

float MarvGame::MarvGame::getSpeed(){
	return speed;
}

void MarvGame::MarvGame::scoreUp(){
	audio.play({ { 400, 600, 75 },
				 { 0, 0, 50 },
				 { 500, 900, 75 } });
//	RGB.blink(Pixel::Green);

	score++;
	robot->setSpeed((float)StartingLightsSpeed * (1.0 - (float)score / (float)scoreMax));

	std::string scoreText = "Score:" + std::to_string(score) + "/" + std::to_string(scoreMax);
	scoreTextRC->setText(scoreText);
}

void MarvGame::MarvGame::handleInput(const Input::Data& data){
	if(isDone || endPauseState != Running) return;

	if(data.action == Input::Data::Press){
		if(data.btn == Input::Down){
			duck->duckPressed();
		}
		if(data.btn == Input::Up){
			duck->jumpPressed();
		}
	}else if(data.action == Input::Data::Release){
		if(data.btn == Input::Down){
			duck->duckReleased();
		}
	}

}
