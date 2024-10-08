#include "Asteroids.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/MultiRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Collision/CircleCC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "Util/Services.h"
#include <esp_random.h>

Asteroids::Asteroids::Asteroids(Sprite& canvas) : Game(canvas, Games::Robby, "/Games/Robby", {
		{ "/bg.raw", {}, true },
		{ asteroidIcons[0].path, {}, true },
		{ asteroidIcons[1].path, {}, true },
		{ asteroidIcons[2].path, {}, true },
		{ asteroidIcons[3].path, {}, true },
		{ asteroidIcons[4].path, {}, true },
		{ asteroidIcons[5].path, {}, true },
		{ "/dead.gif", {}, true },
		{ "/bullet.gif", {}, true },
		RES_HEART,
		RES_GOBLET
}), wrapWalls({ .top =  { nullptr, std::make_unique<RectCC>(glm::vec2{ wrapWallsSize.x, 100 }) },
					  .bot =  { nullptr, std::make_unique<RectCC>(glm::vec2{ wrapWallsSize.x, 100 }) },
					  .left =  { nullptr, std::make_unique<RectCC>(glm::vec2{ 100, wrapWallsSize.y }) },
					  .right =  { nullptr, std::make_unique<RectCC>(glm::vec2{ 100, wrapWallsSize.y }) }
			  }){
	wrapWalls.top.setPos(glm::vec2{ 0, -100 } - (2 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 1));
	wrapWalls.bot.setPos(glm::vec2{ -(2 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 1), 128 + (2 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 1) });
	wrapWalls.left.setPos(glm::vec2{ -100, 0 } - (2 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 1));
	wrapWalls.right.setPos(
			glm::vec2{ 128 + (2 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 1), -(2 * asteroidRadius[(uint8_t) AsteroidSize::Large] + 1) });

	robot = std::make_shared<RoboCtrl::Robby>();
	setRobot(robot);
}

void Asteroids::Asteroids::onLoad(){
	auto pat = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/dead.gif")),
			std::make_unique<CircleCC>(PlayerRadius, glm::vec2{ 36 / 2, 38 / 2 }));

	playerAnim = std::static_pointer_cast<AnimRC>(pat->getRenderComponent());
	playerAnim->setLoopMode(GIF::Single);
	playerAnim->reset();
	playerAnim->start();
	playerAnim->stop();

	addObject(pat);
	player.setObj(pat);
	pat->setPos({ startPosition.x, 0 });


	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	addObject(bg);
	bg->getRenderComponent()->setLayer(-1);

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());

	originalBullet = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/bullet.gif")),
			nullptr
	);
	originalBullet->setPos(-50, -50);
	auto anim = std::static_pointer_cast<AnimRC>(originalBullet->getRenderComponent());
	anim->setLoopMode(GIF::Infinite);
	anim->start();
	addObject(originalBullet);
}

void Asteroids::Asteroids::onLoop(float deltaTime){
	switch(state){
		case Intro:{
			introTimer += deltaTime;
			float progress = introTimer / introTime;

			if(progress >= 1.f){
				player.getObj()->setPos(startPosition);
				state = Running;
				nextLevel();
			}
			progress = sin((progress * M_PI) / 2); //easing function
			float y = 128 - progress * (128 - startPosition.y);

			player.getObj()->setPos({ startPosition.x, y });
			break;
		}

		case Running:
			if(leftHold && !rightHold){
				player.leftTurn(deltaTime);
			}else if(rightHold && !leftHold){
				player.rightTurn(deltaTime);
			}

			updateInvincibility(deltaTime);
			updateBullets(deltaTime);
			updateAsteroids(deltaTime);


			if(asteroidPool.empty()){
				if(level == MaxLevel){
					robot->setPeriod(WinLosePeriod);
					state = Win;
					Sound s = { { 200,  200,  80 },
								{ 400,  400,  80 },
								{ 600,  600,  80 },
								{ 0,    0,    80 },
								{ 400,  400,  80 },
								{ 600,  600,  80 },
								{ 800,  800,  80 },
								{ 0,    0,    80 },
								{ 600,  600,  80 },
								{ 800,  800,  80 },
								{ 1000, 1000, 80 } };
					audio.play(s);
					return;
				}
				nextLevel();
			}

			updateRobotPeriod();
			break;

		case DeathAnim:
			updateAsteroids(deltaTime);
			updateBullets(deltaTime);
			break;

		case DeathPause:
			updateAsteroids(deltaTime);
			updateBullets(deltaTime);

			deathTimer += deltaTime;
			if(deathTimer >= deathPauseTime){
				exit();
			}
			break;

		case Win:
			updateBullets(deltaTime);
			glm::vec2 direction = { cos(M_PI * (player.getAngle() - 90.f) / 180.0), sin(M_PI * (player.getAngle() - 90.f) / 180.0) };

			winTimer += deltaTime;
			if(winTimer > 1.f){
				player.getObj()->setPos(player.getObj()->getPos() + direction * winAcceleration * (float) pow(winTimer - 1.0f, 2));
			}
			if(winTimer >= winTime){
				if(noMiss){
					addAchi(Achievement::Robby_sharp, 1);
				}

				if(life == 3){
					addAchi(Achievement::Robby_asteroid, 1);
				}

				exit();
			}
			break;
	}
}

void Asteroids::Asteroids::handleInput(const Input::Data& data){
	if(state != Running) return;

	if(data.action == Input::Data::Press){
		switch(data.btn){
			case Input::Left:
				leftHold = true;
				break;

			case Input::Right:
				rightHold = true;
				break;

			case Input::A:
				shootBullet();
				break;

			default:
				break;
		}
	}else if(data.action == Input::Data::Release){
		switch(data.btn){
			case Input::Left:
				leftHold = false;
				break;

			case Input::Right:
				rightHold = false;
				break;

			default:
				break;
		}
	}
}

uint32_t Asteroids::Asteroids::getXP() const{
	return ((float)score / (float)MaxScore) * 150.0f;
}

void Asteroids::Asteroids::onStop(){
	handleInput({ Input::Button::Left, Input::Data::Release });
	handleInput({ Input::Button::Right, Input::Data::Release });
}

void Asteroids::Asteroids::updateBullets(float deltaTime){
	for(auto& bullet : bulletPool){
		bullet.gObj->setPos(bullet.gObj->getPos() + bullet.velocity * deltaTime);
	}
}

void Asteroids::Asteroids::shootBullet(){
	if(bulletPool.size() >= 4) return;

	audio.play({ { 450, 300, 100 } });
	std::initializer_list<glm::vec2> polyPoints = { { 0, 6 },
													{ 0, 0 },
													{ 3, 0 },
													{ 3, 6 } };
	auto bullet = std::make_shared<GameObject>(std::make_unique<MultiRC>(originalBullet->getRenderComponent()),
											   std::make_unique<PolygonCC>(polyPoints, glm::vec2{ 1.5, 3 }));
	addObject(bullet);

	glm::vec2 center = player.getObj()->getPos() + glm::vec2{ 34 / 2, 36 / 2 };
	glm::vec2 direction = { cos(M_PI * (player.getAngle() + 90.f) / 180.0), sin(M_PI * (player.getAngle() + 90.f) / 180.0) };
	glm::vec2 bulletPos = (direction * PlayerRadius) + center;
	glm::vec2 speed = direction * bulletSpeed;

	bullet->setPos(bulletPos);
	bullet->setRot(player.getAngle());

	Bullet b = { bullet, speed };
	bulletPool.push_back(b);

	for(auto& asteroid : asteroidPool){
		collision.addPair(*asteroid.gObj, *bullet, [this, b, asteroid](){
			bulletPool.erase(std::remove(bulletPool.begin(), bulletPool.end(), b), bulletPool.end());
			removeObject(b.gObj);

			asteroidHit(asteroid);
			scoreDisplay->setScore(++score);
		});
	}

	collision.wallsAll(*bullet, [this, b](){
		bulletPool.erase(std::remove(bulletPool.begin(), bulletPool.end(), b), bulletPool.end());
		removeObject(b.gObj);
		noMiss = false;
	});
}

void Asteroids::Asteroids::createAsteroid(Asteroids::Asteroids::AsteroidSize size, glm::vec2 pos){
	GameObjPtr asteroid;
	uint8_t spriteIndex = ((esp_random() % 2) * (uint8_t) AsteroidSize::Count) + (uint8_t) size;
	asteroid = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(asteroidIcons[spriteIndex].path), asteroidIcons[spriteIndex].dim),
			std::make_unique<CircleCC>(asteroidRadius[(uint8_t) size], glm::vec2{ asteroidRadius[(uint8_t) size], asteroidRadius[(uint8_t) size] }));

	addObject(asteroid);
	asteroid->setPos(pos);

	//random direction, avoid right angles since they can keep the asteroids off-screen for long durations
	float angle = static_cast <float> (esp_random()) / (static_cast <float> (RAND_MAX / 360.0f));
	float rightAngleOffset = 15;
	if(fmod(angle, 90) <= rightAngleOffset){
		angle += rightAngleOffset;
	}else if(fmod(angle, 90) >= 90 - rightAngleOffset){
		angle -= rightAngleOffset;
	}

	glm::vec2 direction = { cos(M_PI * angle / 180.0), sin(M_PI * angle / 180.0) };
	glm::vec2 speed = direction * asteroidSpeed[(uint8_t) size];

	Asteroid a = { asteroid, speed, size };
	asteroidPool.push_back(a);

	for(auto& bullet : bulletPool){
		collision.addPair(*asteroid, *bullet.gObj, [this, bullet, a](){
			bulletPool.erase(std::remove(bulletPool.begin(), bulletPool.end(), bullet), bulletPool.end());
			removeObject(bullet.gObj);

			asteroidHit(a);
		});
	}

	collision.addPair(*asteroid, *player.getObj(), [this, a](){
		if(invincible) return;

		asteroidHit(a);
		playerHit();
	});


	//wrapping around screen
	collision.addPair(*asteroid, wrapWalls.top, [asteroid](){
		asteroid->setPos({ asteroid->getPos().x, 128.0f });
	});
	collision.addPair(*asteroid, wrapWalls.bot, [asteroid](){
		asteroid->setPos({ asteroid->getPos().x, -(2 * asteroidRadius[(uint8_t) AsteroidSize::Large]) });
	});
	collision.addPair(*asteroid, wrapWalls.left, [asteroid](){
		asteroid->setPos({ 128.0f, asteroid->getPos().y });
	});
	collision.addPair(*asteroid, wrapWalls.right, [asteroid](){
		asteroid->setPos({ -(2 * asteroidRadius[(uint8_t) AsteroidSize::Large]), asteroid->getPos().y });
	});
}

void Asteroids::Asteroids::updateAsteroids(float deltaTime){
	for(auto& asteroid : asteroidPool){
		asteroid.gObj->setPos(asteroid.gObj->getPos() + asteroid.velocity * deltaTime);
	}
}

void Asteroids::Asteroids::asteroidHit(const Asteroids::Asteroids::Asteroid& asteroid){
//	RGB.blink(Pixel::Green);
	audio.play({ { 100, 100, 50 } });

	switch(asteroid.size){
		case AsteroidSize::Large:
			createAsteroid(AsteroidSize::Medium,
						   asteroid.gObj->getPos() + (asteroidRadius[(uint8_t) AsteroidSize::Large] - asteroidRadius[(uint8_t) AsteroidSize::Medium]));
			createAsteroid(AsteroidSize::Medium,
						   asteroid.gObj->getPos() + (asteroidRadius[(uint8_t) AsteroidSize::Large] - asteroidRadius[(uint8_t) AsteroidSize::Medium]));
			break;
		case AsteroidSize::Medium:
			createAsteroid(AsteroidSize::Small,
						   asteroid.gObj->getPos() + (asteroidRadius[(uint8_t) AsteroidSize::Medium] - asteroidRadius[(uint8_t) AsteroidSize::Small]));
			createAsteroid(AsteroidSize::Small,
						   asteroid.gObj->getPos() + (asteroidRadius[(uint8_t) AsteroidSize::Medium] - asteroidRadius[(uint8_t) AsteroidSize::Small]));
			break;
		case AsteroidSize::Small:
			break;
		default:
			break;
	}
	asteroidPool.erase(std::remove(asteroidPool.begin(), asteroidPool.end(), asteroid), asteroidPool.end());
	removeObject(asteroid.gObj);
}

void Asteroids::Asteroids::updateInvincibility(float delta){
	if(!invincible) return;

	invincibilityTime += delta;

	if((int) (invincibilityTime / invincibilityBlinkDuration) % 2 == 0){
		player.getObj()->getRenderComponent()->setVisible(false);
	}else{
		player.getObj()->getRenderComponent()->setVisible(true);
	}

	if(invincibilityTime >= invincibilityDuration){
		invincibilityTime = 0;
		invincible = false;
		player.getObj()->getRenderComponent()->setVisible(true);
	}
}

void Asteroids::Asteroids::playerHit(){
//	RGB.blinkTwice(Pixel::Red);

	life--;
	hearts->setLives(life);
	if(life == 0){
		audio.play({ { 320, 320, 200 },
					 { 0,   0,   50 },
					 { 180, 180, 200 },
					 { 0,   0,   50 },
					 { 220, 220, 200 },
					 { 0,   0,   50 },
					 { 100, 100, 500 } });
		gameOver();
		return;
	}
	audio.play({ { 300, 300, 50 },
				 { 0,   0,   50 },
				 { 300, 300, 50 } });
	invincible = true;
}

void Asteroids::Asteroids::nextLevel(){
	level++;

	for(uint8_t i = 0; i < level; i++){
		spawnRandomAsteroid();
	}
}

void Asteroids::Asteroids::spawnRandomAsteroid(){
	glm::vec2 pos;
	//New asteroids will only be spawned barely outside the screen area:
	//x in range (-2*asteroidRadius[Large], 128), y either -2*asteroidRadius[Large] or 128
	//y in range (-2*asteroidRadius[Large], 128), x either -2*asteroidRadius[Large] or 128

	//top left corner of rectangle outside of the screen, wider by 2*radius[Large] than screen
	glm::vec2 topLeft = { -2 * asteroidRadius[(uint8_t) AsteroidSize::Large], -2 * asteroidRadius[(uint8_t) AsteroidSize::Large] };

	//pick border for asteroid to spawn on
	enum class Border : uint8_t {
		Up, Down, Left, Right
	} side = static_cast<Border>(esp_random() % 4);

	if(side == Border::Up || side == Border::Down){
		float xpos = static_cast <float> (esp_random()) / (static_cast <float> (RAND_MAX / (128.0f - topLeft.x)));

		if(side == Border::Up){
			pos = { topLeft.x + xpos, topLeft.y };
		}else if(side == Border::Down){
			pos = { topLeft.x + xpos, 128.0f };
		}
	}else if(side == Border::Left || side == Border::Right){
		float ypos = static_cast <float> (esp_random()) / (static_cast <float> (RAND_MAX / (128.0f - topLeft.y)));

		if(side == Border::Left){
			pos = { topLeft.x, topLeft.y + ypos };
		}else if(side == Border::Right){
			pos = { 128.0f, topLeft.y + ypos };
		}
	}

	createAsteroid(AsteroidSize::Large, pos);
}

void Asteroids::Asteroids::gameOver(){
	robot->setPeriod(WinLosePeriod);

	for(auto& asteroid : asteroidPool){
		collision.removePair(*asteroid.gObj, *player.getObj());
		for(auto& bullet : bulletPool){
			collision.removePair(*asteroid.gObj, *bullet.gObj);
		}
	}

	state = DeathAnim;
	playerAnim->reset();
	playerAnim->start();
	playerAnim->setLoopDoneCallback([this](uint32_t){
		state = DeathPause;
	});
}

void Asteroids::Asteroids::updateRobotPeriod(){
	float minDistance = MaxAsteroidDistance;
	for(auto& asteroid : asteroidPool){
		const auto r = asteroidRadius[(uint8_t) asteroid.size];
		float distance = glm::distance(player.getObj()->getPos() + glm::vec2{ PlayerRadius, PlayerRadius },
									   asteroid.gObj->getPos() + glm::vec2{ r, r });
		distance += r;
		if(distance < minDistance){
			minDistance = distance;
		}
	}
	minDistance = std::clamp(minDistance, MinAsteroidDistance, MaxAsteroidDistance);
	float period = MinRobotPeriod + (MaxRobotPeriod - MinRobotPeriod) * pow((minDistance / MaxAsteroidDistance), 2);
	robot->setPeriod(period);
}
