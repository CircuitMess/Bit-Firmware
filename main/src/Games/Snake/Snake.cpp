#include "Snake.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "Services/HighScoreManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/Game/AwardsScreen.h"
#include <esp_random.h>

Snake::Snake(Sprite& canvas) : Game(canvas, Games::Snake, "/Games/Snake", {
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

	speed = { 0, -baseSpeed };

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
		moveBuffer += deltaTime * baseSpeed;
		if(moveBuffer >= TileDim.x){
			moveSnake(speed);
			moveBuffer -= TileDim.x;

			if(inputInterrupt){
				inputInterrupt = false;
			}
		}
	}else if(state == State::GameOver){
		if(snake.empty()){
			state = State::DonePause;
			gameWinCounter = 0;
			return;
		}

		moveBuffer += deltaTime * baseSpeed;
		if(moveBuffer >= TileDim.x){
			removeObject(snake.back());
			snake.pop_back();
			moveBuffer = 0;
		}
	}else if(state == State::GameWin){
		gameWinCounter += deltaTime;
		if(gameWinCounter >= GameWinPause){
			exit();
			return;
		}
	}else if(state == State::DonePause){
		gameWinCounter += deltaTime;
		if(gameWinCounter >= donePause){

			exit();
			return;
		}
	}
}

void Snake::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;

	if(state == State::DonePause || state == State::GameWin || state == State::GameOver){
		return;
	}

	if(data.btn == Input::A || data.btn == Input::B || data.btn == Input::Menu) return;

	auto oldSpeed = speed;

	switch(data.btn){
		case Input::Up:
			if(speed.y != 0) return;
			speed = { 0, -baseSpeed };
			break;
		case Input::Down:
			if(speed.y != 0) return;
			speed = { 0, baseSpeed };
			break;
		case Input::Left:
			if(speed.x != 0) return;
			speed = { -baseSpeed, 0 };
			break;
		case Input::Right:
			if(speed.x != 0) return;
			speed = { baseSpeed, 0 };
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
	audio.play({ { 100, 100, 75 } });

}

uint32_t Snake::getXP() const{
	if(score <= XPCutoff){
		return score * 2;
	}else{
		return (score - XPCutoff) + XPCutoff * 2;
	}
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
	const uint16_t duration = std::clamp((int) score * 100, 400, 1400);
	audio.play({ { 100, 80,  100 },
				 { 0,   0,   250 },
				 { 700, 300, static_cast<uint16_t>(duration / 4) },
				 { 600, 200, static_cast<uint16_t>(duration / 4) },
				 { 500, 100, static_cast<uint16_t>(duration / 2) } });
	donePause = 1.5;
}

void Snake::foodEaten(bool initial){
	if(!initial){

		static const Sound EatSounds[] = {
				{ { 250, 200, 50 }, { 400, 700, 50 } },
				{ { 150, 150, 75 }, { 0,   0,   75 }, { 150, 150, 75 }, { 200, 700, 100 } },
				{ { 150, 150, 75 }, { 0,   0,   75 }, { 150, 150, 75 }, { 200, 700, 100 } },
				{ { 400, 600, 75 }, { 0,   0,   50 }, { 500, 900, 75 } }
		};
		audio.play(EatSounds[esp_random() % (sizeof(EatSounds) / sizeof(EatSounds[0]))]);

		scoreElement->setScore(++score);
		addSegment();

		checkAchi();

		if(score >= GridDim.x * GridDim.y){
			gameWinCounter = 0;
			audio.stop();
			audio.play({ { 600, 400,  200 },
						 { 400, 1000, 400 } });
			state = State::GameWin;
			return;
		}

		baseSpeed += SpeedIncrement;
	}
	const auto& foodIndex = esp_random() % (sizeof(Foods) / sizeof(Foods[0]));
	std::static_pointer_cast<StaticRC>(food->getRenderComponent())->setFile(getFile(Foods[foodIndex].path), Foods[foodIndex].dim);
	std::static_pointer_cast<RectCC>(food->getCollisionComponent())->setOffset((Foods[foodIndex].dim - HitboxDim) / (short) 2);

	//generate random pos for food but not on already existing snake positions, also not on edge of grid
	auto randId = esp_random() % ((GridDim.x - 2) * (GridDim.y - 2) - snake.size()); //generates coords on 24x24 subgrid
	std::set<uint32_t> snakeSpaces;
	for(const auto& segment : snake){
		auto pos = segment->getPos();
		pos -= PaddingGap;
		pos /= TileDim.x;
		snakeSpaces.insert(pos.x + pos.y * GridDim.x);
	}
	for(const auto& snakeSpace : snakeSpaces){
		const auto shiftedX = (randId % (GridDim.x - 2)) + 1;
		const auto shiftedY = (randId / (GridDim.y - 2)) + 1;
		const auto shiftedId = shiftedX * shiftedY; //convert subgrid coords to global grid coords (to compare with snake body)
		if(shiftedId < snakeSpace){
			break;
		}
		randId++;
	}


	auto pos = PixelDim{ (randId % (GridDim.x - 2)) + 1, (randId / (GridDim.y - 2)) + 1 };
	pos.x *= TileDim.x;
	pos.y *= TileDim.y;
	pos += PaddingGap;
	pos -= ((Foods[foodIndex].dim - TileDim) / (short) 2);
	food->setPos(pos);

}

void Snake::checkAchi(){
	setAchiIfBigger(Achievement::Snake_b, score);
	setAchiIfBigger(Achievement::Snake_s, score);
	setAchiIfBigger(Achievement::Snake_g, score);

	if(score >= GridDim.x * GridDim.y - 20){
		setAchiIfBigger(Achievement::Snake_fill, 1);
	}
}
