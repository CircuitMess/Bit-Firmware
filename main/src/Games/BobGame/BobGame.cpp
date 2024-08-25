#include "BobGame.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include <time.h>
#include <esp_random.h>

BobGame::BobGame::BobGame(Sprite& canvas) : Game(canvas, Games::Bob, "/Games/Bob", {
		{ "/bg.raw", {}, true },
		{ "/Bomb.raw", {}, true },
		{ "/Dynamite.raw", {}, true },
		{ "/Bulb-g.raw", {}, true },
		{ "/Bulb-y.raw", {}, true },
		{ "/BarFrame.raw", {}, true },
		RES_HEART,
		{ "/dead.gif", {}, true },
		{ "/eatBad.gif", {}, false },
		{ "/win.gif", {}, false },
		{ "/eat.gif", {}, false }
}){
	robot = std::make_shared<RoboCtrl::Bob>();
	setRobot(robot);
}

void BobGame::BobGame::onLoad(){
	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(-2);

	collectorBot = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(PixelDim{ 128, 15 })
	);
	collectorBot->setPos({ 0, 128 + 30 });
	addObject(collectorBot);

	player = std::make_unique<Player>(getFile("/eat.gif"), getFile("/eatBad.gif"), getFile("/win.gif"), getFile("/dead.gif"));
	addObject(player->getGameObject());
	player->getGameObject()->getRenderComponent()->setLayer(1);

	addTemplate("/Bulb-g.raw", PixelDim{ 13, 19 }, 10);
	addTemplate("/Bulb-y.raw", PixelDim{ 13, 19 }, 5);
	addTemplate("/Bomb.raw", PixelDim{ 14, 22 }, 0);
	addTemplate("/Dynamite.raw", PixelDim{ 5, 20 }, 0);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 10, 5 });
	addObject(hearts->getGO());

	auto barRC = std::make_unique<SpriteRC>(PixelDim{ 8, 122 });
	hungerBar = barRC->getSprite();
	barRC->setLayer(2);
	auto barGO = std::make_shared<GameObject>(
			std::move(barRC),
			nullptr
	);
	addObject(barGO);
	barGO->setPos({ 1, 2 });
	hungerBar->clear(TFT_TRANSPARENT);
	Display::drawFile(*hungerBar, getFile("/BarFrame.raw"), 0, 0, 6, 120);
}

void BobGame::BobGame::onLoop(float deltaTime){
	if(state != Running){
		if(state != WinPause && state != DeadPause) return;
		donePauseCounter += deltaTime;
		if(donePauseCounter >= DonePause){
			exit();
		}
		return;
	}

	player->loop(deltaTime);
	timeToSpawn += deltaTime;
	if(timeToSpawn >= spawnInterval){
		spawnRandom();
		timeToSpawn -= spawnInterval;
	}

	if(movingObjects.empty()) return;

	for(const auto& object : movingObjects){
		int y = deltaTime * object.second + object.first->getPos().y;
		int x = object.first->getPos().x;
		object.first->setPos({ x, y });
	}
}

void BobGame::BobGame::onStop(){
	player->btnReleased(Input::Left);
	player->btnReleased(Input::Right);
}

uint32_t BobGame::BobGame::getXP() const{
	return ((float)hungerMeter / (float)hungerMeterMax) * 150.0f;
}

void BobGame::BobGame::addTemplate(std::string file, PixelDim dim, int value){
	Template temp{ file, dim, value };
	if(value > 0){
		foods.push_back(temp);
	}else{
		bombs.push_back(temp);
	}
}

void BobGame::BobGame::spawnRandom(){
	if(foodCounter == FoodInSet && bombCounter == BombInSet){
		foodCounter = bombCounter = 0;
	}
	int randNum = esp_random() % (101);
	if((randNum <= 75 && foodCounter < FoodInSet) || bombCounter == BombInSet){
		foodCounter++;
		int pick = esp_random() % foods.size();
		spawnItem(foods[pick]);
	}else{
		bombCounter++;
		int pick = esp_random() % bombs.size();
		spawnItem(bombs[pick]);
	}
}

void BobGame::BobGame::spawnItem(BobGame::BobGame::Template temp){
	int randPos = esp_random() % (128 - temp.dim.x - 10) + 10; //10 - so that it doesn't spawn on the bar
	auto go = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(temp.path), temp.dim),
			std::make_unique<RectCC>(temp.dim)
	);
	addObject(go);
	float speed = (esp_random() % (speedMax - speedMin + 1)) + speedMin;

	movingObjects.insert(std::make_pair(go, speed));
	go->setPos({ randPos, -temp.dim.y });
	Item item{ go, temp.value };
	collision.addPair(*player->getGameObject(), *item.go, [this, item](){
		collisionHandler(item);
	});
	collision.addPair(*collectorBot, *item.go, [this, item](){
		movingObjects.erase(item.go);
		removeObject(item.go);
	});
}

void BobGame::BobGame::collisionHandler(Item item){
	if(state != Running) return;
	if(player->isEatingBad() && item.value == 0){
		return;
	}

	removeObject(item.go);
	player->startEating(item.value);
	if(item.value > 0){
//		RGB.blink(Pixel::Green);
		hungerMeter = std::min(hungerMeter + item.value, hungerMeterMax);
		drawBar();
		if(hungerMeter >= hungerMeterMax){
			robot->blinkContinuousFast();
			Sound s = { { 400, 600,  200 },
						{ 0,   0,    50 },
						{ 400, 800,  200 },
						{ 0,   0,    50 },
						{ 400, 1000, 200 },
						{ 0,   0,    150 },
						{ 800, 1000, 50 } };
			audio.play(s);
			player->filled(this);
			state = Win;

			if(lives == 3){
				addAchi(Achievement::Bob_bombs, 1);
			}
		}else{
			robot->blink();
			audio.play({ { 250, 200, 50 },
						 { 400, 700, 50 } });
		}
	}else{
//		RGB.blink(Pixel::Red);
		lives--;
		if(lives > 0){
			audio.play({ { 80,  300, 50 },
						 { 0,   0,   50 },
						 { 200, 50,  100 } });
			robot->blinkTwice();
		}
		hearts->setLives(lives);
	}
	if(lives <= 0){
		robot->blinkContinuousSlow();
		player->killed(this);
		audio.play({ { 300, 400, 100 },
					 { 400, 300, 100 },
					 { 200, 300, 100 },
					 { 300, 200, 100 },
					 { 80,  80,  300 } });
		state = Dead;
	}

	movingObjects.erase(item.go);
}

typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

static rgb hsv2rgb(hsv in);

rgb hsv2rgb(hsv in){
	double hh, p, q, t, ff;
	long i;
	rgb out;

	if(in.s <= 0.0){       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if(hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long) hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch(i){
		case 0:
			out.r = in.v;
			out.g = t;
			out.b = p;
			break;
		case 1:
			out.r = q;
			out.g = in.v;
			out.b = p;
			break;
		case 2:
			out.r = p;
			out.g = in.v;
			out.b = t;
			break;

		case 3:
			out.r = p;
			out.g = q;
			out.b = in.v;
			break;
		case 4:
			out.r = t;
			out.g = p;
			out.b = in.v;
			break;
		case 5:
		default:
			out.r = in.v;
			out.g = p;
			out.b = q;
			break;
	}
	return out;
}

void BobGame::BobGame::drawBar(){
	float fillPercent = ((float) hungerMeter / (float) hungerMeterMax) * 116.0f;

	float difference = abs(116 - fillPercent);
	double hue = (116.0f - difference) / 100.0 * 60.0 / 255.0 * 360;
	rgb rgbColor0 = hsv2rgb({ hue, 1.0, 1.0 });
	uint16_t c0 = lgfx::color565(rgbColor0.r * 255.0, rgbColor0.g * 255.0, rgbColor0.b * 255.0);

	hungerBar->fillRect(2, 2 + 116 - std::round(fillPercent), 2, std::round(fillPercent), c0);
}

void BobGame::BobGame::handleInput(const Input::Data& data){
	if(data.action == Input::Data::Press){
		player->btnPressed(data.btn);
	}else if(data.action == Input::Data::Release){
		player->btnReleased(data.btn);
	}
}
