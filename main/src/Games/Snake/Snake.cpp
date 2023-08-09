#include "Snake.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"

Snake::Snake(Sprite& canvas) : Game(canvas, "/Games/Snake", {
		{ "/bg.raw", {}, true },
		{ Foods[0].path, {}, true },
		{ Foods[1].path, {}, true },
		{ Foods[2].path, {}, true },
		{ Foods[3].path, {}, true },
		{ Foods[4].path, {}, true },
		{ Foods[5].path, {}, true },
		{ Foods[6].path, {}, true },
		{ Foods[7].path, {}, true },
		{ Foods[8].path, {}, true },
		{ Foods[9].path, {}, true },
		RES_GOBLET
}){

}

void Snake::onLoad(){
	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(-2);

	auto head = std::make_shared<GameObject>(
			std::make_unique<SpriteRC>(TileDim),
			std::make_unique<RectCC>(HitboxDim, (TileDim - HitboxDim) / (short) 2)
	);
	head->setPos({ PaddingGap + 11 * TileDim.x, PaddingGap + 11 * TileDim.y });
	head->getRenderComponent()->setLayer(1);
	headSprite = std::static_pointer_cast<SpriteRC>(head->getRenderComponent())->getSprite();
	snake.push_back(head);
	addObject(head);

	speed = { 0, -BaseSpeed };

	for(int i = 1; i < StartingLength; ++i){
		addSegment();
	}

	scoreElement = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreElement->setScore(score);
	addObject(scoreElement->getGO());
	scoreElement->getGO()->getRenderComponent()->setLayer(2);
	scoreElement->getGO()->setPos({ 128 - (PaddingGap + 3) - 28, (PaddingGap + 3) });

	food = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(Foods[0].path), Foods[0].dim),
			std::make_unique<RectCC>(HitboxDim, (Foods[0].dim - HitboxDim) / (short) 2)
	);
	foodEaten(true);
	addObject(food);

	collision.addPair(*head, *food, [this](){ foodEaten(); });

	drawHead(*headSprite);

	state = State::Running;
}

void Snake::onLoop(float deltaTime){
	if(state == State::Running){
		moveBuffer += deltaTime * BaseSpeed;
		if(moveBuffer >= TileDim.x){
			moveSnake(speed);
			moveBuffer -= TileDim.x;

			if(inputInterrupt){
				inputInterrupt = false;
			}
		}
	}else if(state == State::GameOver){
		if(snake.empty()){
			//pop
			return;
		}

		moveBuffer += deltaTime * BaseSpeed;
		if(moveBuffer >= TileDim.x){
			removeObject(snake.back());
			snake.pop_back();
			moveBuffer = 0;

		}
	}
}

void Snake::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;

	if(state == State::GameOver){
		//pop
		return;
	}

	if(data.btn == Input::A || data.btn == Input::B || data.btn == Input::Menu) return;

	auto oldSpeed = speed;

	switch(data.btn){
		case Input::Up:
			if(speed.y != 0) return;
			speed = { 0, -BaseSpeed };
			break;
		case Input::Down:
			if(speed.y != 0) return;
			speed = { 0, BaseSpeed };
			break;
		case Input::Left:
			if(speed.x != 0) return;
			speed = { -BaseSpeed, 0 };
			break;
		case Input::Right:
			if(speed.x != 0) return;
			speed = { BaseSpeed, 0 };
			break;
		default:
			break;
	}

	if(inputInterrupt){
		moveSnake(oldSpeed);
	}

	inputInterrupt = true;
	moveBuffer = 0;
	drawHead(*headSprite);
	audio.play({ { 100, 100, 100 } });

}

void Snake::drawHead(Sprite& head){
	head.clear(TFT_GREEN);
	if(speed.x > 0){
		head.drawRect(3, 1, 2, 3, TFT_RED);
		head.drawPixel(0, 1, TFT_BLACK);
		head.drawPixel(1, 1, TFT_BLACK);
		head.drawPixel(0, 3, TFT_BLACK);
		head.drawPixel(1, 3, TFT_BLACK);
	}else if(speed.x < 0){
		head.fillRect(0, 1, 2, 3, TFT_RED);
		head.drawPixel(3, 1, TFT_BLACK);
		head.drawPixel(3, 3, TFT_BLACK);
		head.drawPixel(4, 1, TFT_BLACK);
		head.drawPixel(4, 3, TFT_BLACK);
	}else if(speed.y > 0){
		head.fillRect(1, 3, 3, 2, TFT_RED);
		head.drawPixel(1, 0, TFT_BLACK);
		head.drawPixel(3, 0, TFT_BLACK);
		head.drawPixel(1, 1, TFT_BLACK);
		head.drawPixel(3, 1, TFT_BLACK);
	}else if(speed.y < 0){
		head.fillRect(1, 0, 3, 2, TFT_RED);
		head.drawPixel(1, 3, TFT_BLACK);
		head.drawPixel(3, 3, TFT_BLACK);
		head.drawPixel(1, 4, TFT_BLACK);
		head.drawPixel(3, 4, TFT_BLACK);
	}
}

void Snake::addSegment(){
	auto segment = std::make_shared<GameObject>(
			std::make_unique<SpriteRC>(TileDim),
			std::make_unique<RectCC>(HitboxDim, (TileDim - HitboxDim) / (short) 2)
	);
	auto sprite = std::static_pointer_cast<SpriteRC>(segment->getRenderComponent())->getSprite();
	sprite->clear(TFT_GREEN);

	if(speed.x != 0){
		sprite->drawLine(2, 0, 2, 4, TFT_DARKGREEN);
	}else if(speed.y != 0){
		sprite->drawLine(0, 2, 4, 2, TFT_DARKGREEN);
	}


	addObject(segment);
	auto pos = snake.back()->getPos();
	if(speed.x != 0){
		pos.x = pos.x + (speed.x > 0 ? -TileDim.x : TileDim.x);
	}else if(speed.y != 0){
		pos.y = pos.y + (speed.y > 0 ? -TileDim.y : TileDim.y);
	}
	segment->setPos(pos);
	snake.push_back(segment);

	if(snake.size() >= 3){
		collision.addPair(*snake.front(), *segment, [this](){ gameOver(); });
	}
}

void Snake::moveSnake(glm::vec2 speed){
	auto move = glm::normalize(speed);
	move *= TileDim.x;

	for(int i = snake.size() - 1; i > 0; --i){
		auto motion = snake[i - 1]->getPos() - snake[i]->getPos();
		auto sprite = std::static_pointer_cast<SpriteRC>(snake[i]->getRenderComponent())->getSprite();
		sprite->clear(TFT_GREEN);
		if(motion.x != 0){
			sprite->drawLine(2, 0, 2, 4, TFT_DARKGREEN);
		}else if(motion.y != 0){
			sprite->drawLine(0, 2, 4, 2, TFT_DARKGREEN);
		}
		snake[i]->setPos(snake[i - 1]->getPos());
	}
	snake[0]->setPos(snake[0]->getPos() + move);

	for(auto segment : snake){
		auto pos = segment->getPos();
		if(pos.x < PaddingGap){
			pos.x = PaddingGap + (GridDim.x - 1) * TileDim.x;
		}else if(pos.x > PaddingGap + (GridDim.x - 1) * TileDim.x){
			pos.x = PaddingGap;
		}

		if(pos.y < PaddingGap){
			pos.y = PaddingGap + (GridDim.y - 1) * TileDim.y;
		}else if(pos.y > PaddingGap + (GridDim.y - 1) * TileDim.y){
			pos.y = PaddingGap;
		}
		segment->setPos(pos);
	}
}

void Snake::gameOver(){
	state = State::GameOver;
	audio.play({ { 400, 300, 200 },
				 { 0,   0,   50 },
				 { 300, 200, 200 },
				 { 0,   0,   50 },
				 { 200, 50,  400 } });
}

void Snake::foodEaten(bool initial){
	if(!initial){
		audio.play({ { 100, 100, 100 } });
		scoreElement->setScore(++score);
		addSegment();
	}
	const auto& foodIndex = rand() % (sizeof(Foods) / sizeof(Foods[0]));
	std::static_pointer_cast<StaticRC>(food->getRenderComponent())->setFile(getFile(Foods[foodIndex].path), Foods[foodIndex].dim);
	std::static_pointer_cast<RectCC>(food->getCollisionComponent())->setOffset((Foods[foodIndex].dim - HitboxDim) / (short) 2);

	//generate random pos for food but not on already existing snake positions
	auto randId = rand() % (GridDim.x * GridDim.y - snake.size());
	std::set<uint32_t> snakeSpaces;
	for(const auto& segment : snake){
		auto pos = segment->getPos();
		pos -= PaddingGap;
		pos /= TileDim.x;
		snakeSpaces.insert(pos.x + pos.y * GridDim.x);
	}
	for(const auto& num : snakeSpaces){
		if(randId < num){
			break;
		}
		randId++;
	}


	auto pos = PixelDim{ randId % GridDim.x, randId / GridDim.x };
	pos.x *= TileDim.x;
	pos.y *= TileDim.y;
	pos += PaddingGap;
	pos -= ((Foods[foodIndex].dim - TileDim) / (short) 2);
	food->setPos(pos);

}
