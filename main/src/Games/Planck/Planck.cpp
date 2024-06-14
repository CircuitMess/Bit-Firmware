#include "Planck.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"

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
		{"/oil.raw", {}, true},
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

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 12 - 28, 2 });
	addObject(scoreDisplay->getGO());

	car = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/car.raw"), PixelDim{ 26, 30 }),
			std::make_unique<RectCC>(PixelDim{ 26, 30 })
	);
	car->setPos(51.0f, 80.0f);
	car->getRenderComponent()->setLayer(1);
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
			const uint8_t notObstacle = rand() % HorizontalTiles;

			road[i * HorizontalTiles] = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile(notObstacle == 0 ? "/road.raw" : Obstacles[rand() % 6]), PixelDim{ 36, 36 }),
					nullptr
			);
			road[i * HorizontalTiles]->setPos(10.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles]);

			road[i * HorizontalTiles + 1] = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile(notObstacle == 1 ? "/road.raw" : Obstacles[rand() % 6]), PixelDim{ 36, 36 }),
					nullptr
			);
			road[i * HorizontalTiles + 1]->setPos(46.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 1]);

			road[i * HorizontalTiles + 2] = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile(notObstacle == 2 ? "/road.raw" : Obstacles[rand() % 6]), PixelDim{ 36, 36 }),
					nullptr
			);
			road[i * HorizontalTiles + 2]->setPos(82.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 2]);
		}else{
			road[i * HorizontalTiles] = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
					nullptr
			);
			road[i * HorizontalTiles]->setPos(10.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles]);

			road[i * HorizontalTiles + 1] = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
					nullptr
			);
			road[i * HorizontalTiles + 1]->setPos(46.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 1]);

			road[i * HorizontalTiles + 2] = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
					nullptr
			);
			road[i * HorizontalTiles + 2]->setPos(82.0f, 96.0f - i * 36.0f);
			addObject(road[i * HorizontalTiles + 2]);
		}
	}
}

void Planck::Planck::onLoop(float deltaTime){
	Game::onLoop(deltaTime);

	car->setPos(car->getPos() + glm::vec2{ direction, 0.0f } * CarSpeed * deltaTime);

	if(car->getPos().x < 10.0f){
		car->setPosX(10.0f);
	}

	if(car->getPos().x > 118.0f - 26.0f){
		car->setPosX(118.0f - 26.0f);
	}

	for(int i = 0; i < HorizontalTiles * VerticalTiles; ++i){
		road[i]->setPos(road[i]->getPos() + glm::vec2{ 0.0f, 1.0f } * Speed * deltaTime);
	}

	if(road[rowToGenerate * HorizontalTiles]->getPos().y >= 128.0f){
		generateRoad();
	}

	for(int i = 0; i < 14; ++i){
		leftEdge[i]->setPos(leftEdge[i]->getPos() + glm::vec2{ 0.0f, 1.0f } * Speed * deltaTime);
		rightEdge[i]->setPos(rightEdge[i]->getPos() + glm::vec2{ 0.0f, 1.0f } * Speed * deltaTime);

		if(leftEdge[i]->getPos().y >= 128.0f){
			leftEdge[i]->setPosY(leftEdge[i]->getPos().y - 140.0f);
		}

		if(rightEdge[i]->getPos().y >= 128.0f){
			rightEdge[i]->setPosY(rightEdge[i]->getPos().y - 140.0f);
		}
	}
}

void Planck::Planck::handleInput(const Input::Data& data){
	Game::handleInput(data);

	if((data.btn == Input::Button::Right && data.action == Input::Data::Press) || (data.btn == Input::Button::Left && data.action == Input::Data::Release)){
		direction += 1.0f;
	}else if((data.btn == Input::Button::Left && data.action == Input::Data::Press) || (data.btn == Input::Button::Right && data.action == Input::Data::Release)){
		direction -= 1.0f;
	}

	direction = std::min(direction, 1.0f);
	direction = std::max(direction, -1.0f);
}

void Planck::Planck::onStop(){
	Game::onStop();
}

void Planck::Planck::generateRoad(){
	removeObject(road[rowToGenerate * HorizontalTiles]);
	removeObject(road[rowToGenerate * HorizontalTiles + 1]);
	removeObject(road[rowToGenerate * HorizontalTiles + 2]);

	if(lastGenerated){
		road[rowToGenerate * HorizontalTiles] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
				nullptr
		);
		road[rowToGenerate * HorizontalTiles]->setPos(10.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles]);

		road[rowToGenerate * HorizontalTiles + 1] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
				nullptr
		);
		road[rowToGenerate * HorizontalTiles + 1]->setPos(46.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 1]);

		road[rowToGenerate * HorizontalTiles + 2] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile("/road.raw"), PixelDim{ 36, 36 }),
				nullptr
		);
		road[rowToGenerate * HorizontalTiles + 2]->setPos(82.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 2]);

		lastGenerated = false;
	}else{
		const uint8_t notObstacle = rand() % HorizontalTiles;

		road[rowToGenerate * HorizontalTiles] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(notObstacle == 0 ? "/road.raw" : Obstacles[rand() % 6]), PixelDim{ 36, 36 }),
				nullptr
		);
		road[rowToGenerate * HorizontalTiles]->setPos(10.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles]);

		road[rowToGenerate * HorizontalTiles + 1] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(notObstacle == 1 ? "/road.raw" : Obstacles[rand() % 6]), PixelDim{ 36, 36 }),
				nullptr
		);
		road[rowToGenerate * HorizontalTiles + 1]->setPos(46.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 1]);

		road[rowToGenerate * HorizontalTiles + 2] = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(notObstacle == 2 ? "/road.raw" : Obstacles[rand() % 6]), PixelDim{ 36, 36 }),
				nullptr
		);
		road[rowToGenerate * HorizontalTiles + 2]->setPos(82.0f, 96.0f - (VerticalTiles - 1) * 36.0f);
		addObject(road[rowToGenerate * HorizontalTiles + 2]);

		lastGenerated = true;
	}

	rowToGenerate = (rowToGenerate + 1) % VerticalTiles;
}
