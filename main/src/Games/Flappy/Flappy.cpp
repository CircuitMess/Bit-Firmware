#include "Flappy.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Collision/CircleCC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "gtx/vector_angle.hpp"
#include "Services/GameManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/AwardsScreen.h"

constexpr Flappy::ObstacleDesc Flappy::TopObstacles[];
constexpr Flappy::ObstacleDesc Flappy::BotObstacles[];

Flappy::Flappy(Sprite& canvas) : Game(canvas, Games::MrBee, "/Games/Flappy", {
		{ "/bee.gif", {}, true },
		{ "/bg.raw", {}, true },
		RES_HEART,
		RES_GOBLET,
		{ TopObstacles[0].path, {}, true },
		{ TopObstacles[1].path, {}, true },
		{ TopObstacles[2].path, {}, true },
		{ TopObstacles[3].path, {}, true },
		{ TopObstacles[4].path, {}, true },
		{ BotObstacles[0].path, {}, true },
		{ BotObstacles[1].path, {}, true },
		{ BotObstacles[2].path, {}, true },
		{ BotObstacles[3].path, {}, true },
		{ BotObstacles[4].path, {}, true },
		{ BotObstacles[5].path, {}, true } }){

	robot = std::make_shared<RoboCtrl::MrBee>();
	setRobot(robot);
}

void Flappy::onLoad(){
	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-1);
	addObject(bg);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());


	bee = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/bee.gif")),
			std::make_unique<CircleCC>(10, glm::vec2{ 13, 13 })
	);

	anim = std::static_pointer_cast<AnimRC>(bee->getRenderComponent());
	anim->setLoopMode(GIF::Single);

	addObject(bee);
	resetDuck();
}

void Flappy::onStart(){
	anim->start();
}

void Flappy::onStop(){
	anim->stop();
	anim->reset();
}

void Flappy::onLoop(float deltaTime){
	updateDuck(deltaTime);
	updateObstacles(deltaTime);

	if(state == Play){
		if(obstacles.empty() || obstacles.back().top->getPos().x <= 128 - obstacleSpacing){
			createObstaclePair();
		}
	}

	if(!obstacles.empty()){
		if(obstacles.front().bot->getPos().x < -40){
			removeObject(obstacles.front().top);
			removeObject(obstacles.front().bot);
			obstacles.pop_front();
		}
	}

	for(auto& obstacle : obstacles){
		if(obstacle.top->getPos().x + 15 <= duckPosX && !obstacle.passed && state == Play){
			audio.play({ { 600, 900, 50 },
						 { 0,   0,   50 },
						 { 600, 900, 50 } });
			robot->blink();
//			RGB.blink(Pixel::Green);
			score++;
			obstacle.passed = true;
			scoreDisplay->setScore(score);

			speedX += 1;
		}
	}

	if(state == FallOut && obstacles.empty()){
		if(life == 0){
			exit();
		}else{
			resetDuck();
		}
	}
}

void Flappy::updateDuck(float delta){
	if(state != FlyIn && state != Play && state != FallOut) return;

	glm::vec2 pos = bee->getPos();

	if(state == FlyIn){
		entry += delta / entryDuration;
		float tX = entry;
		float tY = 1.f - pow(fmax(0.0f, 1.f - (entry * 1.1f)), 2);

		glm::vec2 newPos = { duckPosX * tX, -20 + (64 + 20) * tY };
		bee->setPos(newPos);

		pos.x -= entry * duckPosX;
		glm::vec2 dir = newPos - pos;
		float angle = glm::orientedAngle(glm::normalize(dir), glm::vec2{ 1, 0 });
		bee->setRot(-glm::degrees(angle));

		newPos.x = duckPosX;
		bee->setPos(newPos);

		if(entry >= 1){
			bee->setPos({ duckPosX, 64 });
			bee->setRot(0);
			velocity.y = 0;
			state = Wait;
		}

		return;
	}

	velocity += delta * glm::vec2{ 0, gravity };
	pos += delta * velocity;
	pos.x = duckPosX;

	if(pos.y < 0){
		velocity.y = 0;
		pos.y = 0;
	}

	float angle = glm::orientedAngle(glm::normalize(velocity), glm::vec2{ 1, 0 });
	angle = -glm::degrees(angle);

	bee->setRot(angle);
	bee->setPos(pos);
}

void Flappy::updateObstacles(float delta){
	auto move = [delta, this](GameObjPtr& obj){
		glm::vec2 pos = obj->getPos();
		pos.x -= delta * speedX;
		obj->setPos(pos);
	};

	for(auto& obstacle : obstacles){
		move(obstacle.top);
		move(obstacle.bot);
	}
}

void Flappy::handleInput(const Input::Data& data){
	if(data.action != Input::Data::Press) return;

	const auto& btn = data.btn;
	if(btn == Input::B){
//		audio.play(Sound{ Chirp{ 400, 350, 50 } });
////		pop();
//		return;
	}else if(btn != Input::A) return;

	if(state == Wait || state == FlyIn){
		anim->setLoopMode(GIF::Single);
		state = Play;
	}

	if(state != Play) return;

	anim->reset();
	anim->start();

	velocity.y = -flapSpeedY;
	audio.play({ { 90,  90,  50 },
				 { 0,   0,   50 },
				 { 200, 600, 100 } });
}

void Flappy::resetDuck(){
	bee->setPos({ 0, -20 });
	bee->setRot(0);
	collision.wallBot(*bee, [this](){ die(); });

	anim->setLoopMode(GIF::Infinite);
	velocity.y = 0;
	state = FlyIn;
	entry = 0;

	robot->ledOff();
}

void Flappy::createObstaclePair(){

	int topi = rand() % (sizeof(TopObstacles) / sizeof(TopObstacles[0]));
	int boti = rand() % (sizeof(BotObstacles) / sizeof(BotObstacles[0]));

	const auto& topDesc = TopObstacles[topi];
	const auto& botDesc = BotObstacles[boti];

	GameObjPtr topObj;
	if(topi == 4){
		topObj = std::make_shared<GameObject>(
				std::make_unique<AnimRC>(getFile(topDesc.path)),
				std::make_unique<PolygonCC>(topDesc.collision)
		);
		std::static_pointer_cast<AnimRC>(topObj->getRenderComponent())->start();
	}else{
		topObj = std::make_shared<GameObject>(
				std::make_unique<StaticRC>(getFile(topDesc.path), topDesc.dim),
				std::make_unique<PolygonCC>(topDesc.collision)
		);
	}

	auto botObj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(botDesc.path), botDesc.dim),
			std::make_unique<PolygonCC>(botDesc.collision)
	);

	int offsetBoth = rand() % 40;

	int offsetTop = rand() % 5;
	int offsetBot = rand() % 5;

	if(topi != 4){
		offsetTop = 0;
		offsetBoth = 0;
	}

	auto gateLowPoint = (128 - botDesc.dim.y) + 10 + offsetBoth + offsetBot;
	auto gateHighPoint = offsetBoth + offsetTop + topDesc.dim.y;
	auto gap = gateLowPoint - gateHighPoint;
	if(gap < MinimumGap){
		if(gateHighPoint > (128 - gateLowPoint)){
			offsetTop -= (MinimumGap - gap);
		}else{
			offsetBot += (MinimumGap - gap);
		}
	}

	topObj->setPos({ 128, offsetBoth + offsetTop });
	botObj->setPos({ 128 + 15, (128 - botDesc.dim.y) + 10 + offsetBoth + offsetBot });

	collision.addPair(*bee, *topObj, [this](){ die(); });
	collision.addPair(*bee, *botObj, [this](){ die(); });

	addObject(topObj);
	addObject(botObj);
	obstacles.push_back({ topObj, botObj, false });
}

void Flappy::die(){
	if(state != Play) return;
//	RGB.blinkTwice(Pixel::Red);

	robot->blinkContinuousFast();
	life--;
	if(life > 0){
		audio.play({ { 60,  60, 50 },
					 { 0,   0,  50 },
					 { 300, 60, 150 } });
	}else{
		audio.play({ { 1000, 150, 1000 },
					 { 0,    0,   50 },
					 { 70,   70,  200 } });
	}
	hearts->setLives(life);

	state = FallOut;

	collision.wallBot(*bee, {});

	for(const auto& obstacle : obstacles){
		collision.removePair(*bee, *obstacle.top);
		collision.removePair(*bee, *obstacle.bot);
	}
}

void Flappy::exit(){
	Game::exited = true;

	if(const GameManager* gm = (GameManager*) Services.get(Service::Games)){
		std::array<HighScore, 5> highScores;

		gm->getHighScores(Games::MrBee, highScores);

		std::sort(highScores.begin(), highScores.end(), [](const HighScore& first, const HighScore& second) {
			return (first.valid && !second.valid) || first.score > second.score;
		});

		for(const HighScore& highScore : highScores){
			if(highScore.valid && highScore.score == score){
				Game::exit();
				return; // Only the first entry of equal high score should be valid
			}
		}

		if(!highScores.back().valid || highScores.back().score < score){
			highScores.back().valid = true;
			highScores.back().score = score;
			highScores.back().id[0] = ' ';
			highScores.back().id[1] = ' ';
			highScores.back().id[2] = ' ';

			auto ui = (UIThread*) Services.get(Service::UI);
			ui->startScreen([highScores](){ return std::make_unique<AwardsScreen>(Games::MrBee, highScores); });
			return;
		}
	}

	Game::exit();
}
