#include "Invaders.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Collision/RectCC.h"
#include "Util/stdafx.h"
#include "Services/HighScoreManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/Game/AwardsScreen.h"
#include <esp_random.h>

const Sound Invaders::Invaders::InvaderDeathSounds[4] = {
		{ { 200, 600, 100 }, { 600, 80,  300 } },
		{ { 200, 600, 100 }, { 600, 200, 100 }, { 100, 500, 100 }, { 500, 100, 200 } },
		{ { 100, 100, 100 }, { 0,   0,   50 },  { 100, 100, 100 }, { 0,   0,   50 }, { 200, 50, 200 } },
		{ { 400, 200, 100 }, { 0,   0,   50 },  { 250, 50,  100 } }
};

Invaders::Invaders::Invaders(Sprite& canvas) : Game(canvas, Games::Resistron, "/Games/Resistron", {
		{ "/bg.raw", {}, true },

		{ InvaderSprites[0].path, {}, true },
		{ InvaderSprites[1].path, {}, true },
		{ InvaderSprites[2].path, {}, true },
		{ InvaderSprites[3].path, {}, true },
		{ "/bullet.raw", {}, true },
		{ "/invBullet.raw", {}, true },

		{ "/hit.gif", {}, true },
		{ "/dead.gif", {}, true },
		RES_HEART,
		RES_GOBLET
}){
	robot = std::make_shared<RoboCtrl::Resistron>();
	setRobot(robot);
}

void Invaders::Invaders::onLoad(){
	bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-1);
	addObject(bg);

	playerObj = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/hit.gif")),
			std::make_unique<RectCC>(PlayerSize, PlayerObjOffset)
	);
	addObject(playerObj);
	playerObj->setPos({ (128 - PlayerSize.x) / 2 - PlayerObjOffset.x, 128 - 1 - PlayerSize.y - PlayerObjOffset.y });

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());
	scoreDisplay->setScore(score);

	player = std::make_unique<Player>(playerObj, this, getFile("/hit.gif"), getFile("/dead.gif"));

	bottomWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 128, 1 })
	);
	bottomWall->setPos(0, 128 + EnemyBulletDim.y + 1);
	addObject(bottomWall);

	topWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 128, 1 })
	);
	topWall->setPos(0, -(PlayerBulletDim.y + 1));
	addObject(topWall);

	killWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 128, 1 })
	);
	killWall->setPos(0, playerObj->getPos().y + PlayerObjOffset.y + 1);
	addObject(killWall);
}

void Invaders::Invaders::onLoop(float deltaTime){
	if(playerBullet){
		float bulletY = playerBullet->getPos().y - deltaTime * PlayerBulletSpeed;
		playerBullet->setPos(playerBullet->getPos().x, bulletY);
	}

	if(enemyBullet){
		float bulletY = enemyBullet->getPos().y + deltaTime * EnemyBulletSpeed;
		enemyBullet->setPos(enemyBullet->getPos().x, bulletY);
	}

	if(player->isDead()){
		endCounter += deltaTime;
		if(endCounter >= EndPause){
			exit();
		}
		return;
	}

	enemyShootTimer += deltaTime;
	const auto enemyShootPeriod = EnemyBaseShootPeriod * pow(0.8, std::max((int) waveCounter - 1, 0));

	if(enemyShootTimer >= enemyShootPeriod){
		if(enemyShoot()){
			enemyShootTimer = 0;
		}
	}

	player->update(deltaTime);

	if(invaders.empty()){
		spawnWave();
	}
	moveInvaders(deltaTime);

}

void Invaders::Invaders::handleInput(const Input::Data& data){
	if(data.action == Input::Data::Press){
		if(data.btn == Input::A){
			shoot();
			return;
		}
		player->btnPressed(data.btn);
	}else if(data.action == Input::Data::Release){
		player->btnReleased(data.btn);
	}
}

void Invaders::Invaders::onStop(){
	player->btnReleased(Input::Left);
	player->btnReleased(Input::Right);
}

uint32_t Invaders::Invaders::getXP() const{
	return std::min(score, (uint32_t)XPCap); //20 for first wave, 29 for second, ...
}

void Invaders::Invaders::shoot(){
	if(playerBullet) return;

	audio.play({ { 500, 900, 50 } });
	robot->blink();

	playerBullet = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bullet.raw"), PlayerBulletDim),
			std::make_unique<RectCC>(PlayerBulletDim)
	);
	playerBullet->setPos(playerObj->getPos().x + PlayerObjOffset.x + PlayerSize.x / 2, playerObj->getPos().y - PlayerBulletDim.y + PlayerObjOffset.y);
	playerBullet->getRenderComponent()->setLayer(1);
	addObject(playerBullet);

	collision.addPair(*playerBullet, *topWall, [this](){
		removeObject(playerBullet);
		playerBullet.reset();
	});

	for(const auto& inv : invaders){
		const uint8_t hp = inv.startingHP;
		const uint8_t type = inv.type;
		std::weak_ptr weakPtr(inv.obj);
		collision.addPair(*playerBullet, *inv.obj, [this, weakPtr, hp, type](){
			removeObject(playerBullet);
			playerBullet.reset();

			auto it = std::find_if(invaders.begin(), invaders.end(), [weakPtr](const Invader& inv){
				return inv.obj == weakPtr.lock();
			});
			auto invObj = weakPtr.lock();
			it->hitpoints--;
			if(it->hitpoints == 0){
				removeObject(invObj);
				invaders.erase(it);
				invaders.shrink_to_fit();
				score += hp;
				scoreDisplay->setScore(score);

				audio.play(InvaderDeathSounds[type]);

				setAchiIfBigger(Achievement::Resistron_b, score);
				setAchiIfBigger(Achievement::Resistron_g, score);
				setAchiIfBigger(Achievement::Resistron_s, score);

			}else{
				audio.play({ { 80, 80, 80 } });
			}
		});
	}

	if(enemyBullet){
		collision.addPair(*playerBullet, *enemyBullet, [this](){
			removeObject(playerBullet);
			playerBullet.reset();
			removeObject(enemyBullet);
			enemyBullet.reset();

			addAchi(Achievement::Resistron_deflect, 1);
		});
	}
}


bool Invaders::Invaders::enemyShoot(){
	if(enemyBullet) return false;
	if(invaders.empty()) return false;

	audio.play({ { 600, 800, 50 },
				 { 800, 600, 50 } });

	const auto& inv = invaders[esp_random() % invaders.size()];
	auto invObj = inv.obj;

	enemyBullet = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/invBullet.raw"), EnemyBulletDim),
			std::make_unique<RectCC>(EnemyBulletDim)
	);
	enemyBullet->setPos(invObj->getPos().x + InvaderSprites[inv.type].dim.x / 2, invObj->getPos().y + InvaderSprites[inv.type].dim.y);
	enemyBullet->getRenderComponent()->setLayer(1);
	addObject(enemyBullet);

	collision.addPair(*enemyBullet, *bottomWall, [this](){
		removeObject(enemyBullet);
		enemyBullet.reset();
	});

	collision.addPair(*enemyBullet, *playerObj, [this](){
		removeObject(enemyBullet);
		enemyBullet.reset();

		hearts->setLives(--lives);
		if(lives == 0){
			player->death();
			audio.play({ { 300, 400, 200 },
						 { 400, 300, 200 },
						 { 200, 300, 200 },
						 { 300, 200, 200 },
						 { 80,  80,  400 } });
		}else{
			player->damage();
			audio.play({ { 100, 100, 50 },
						 { 0,   0,   50 },
						 { 100, 100, 100 },
						 { 0,   0,   100 },
						 { 150, 75,  300 } });
		}
	});


	if(playerBullet){
		collision.addPair(*enemyBullet, *playerBullet, [this](){
			removeObject(enemyBullet);
			enemyBullet.reset();
			removeObject(playerBullet);
			playerBullet.reset();
		});
	}

	return true;
}


void Invaders::Invaders::spawnWave(){
	waveCounter++;
	invadersDirection = MoveDirection::Down;
	nextDirection = MoveDirection::Left;
	waveEnemies = 0;

	/*
	 * wave 1 - 4,3,2,1
	 * wave 2 - 4,4,3,2
	 * wave 3 - 4,4,4,3
	 * wave 4+ - 4,4,4,4 increase all hitpoints by 1 every wave, decrease enemy shooting period
	 */
	float rowY = 0;
	for(int i = 4; i >= 1; i--){
		const uint8_t type = 4 - i;
		const auto numInRow = std::min((int) (i + std::max((int) waveCounter - 1, 0)), 4);
		const auto hp = std::clamp(type + 1 + std::max(((int) waveCounter - 4), 0), 1, 255);

		const auto rowWidth = (InvaderSprites[type].dim.x) * numInRow + (numInRow - 1) * InvadersColumnDistance;
		const float startingX = (128 - rowWidth) / 2;
		rowY -= (InvaderSprites[type].dim.y + InvadersRowDistance);


		for(int j = 0; j < numInRow; ++j){
			auto inv = spawnInvader(type, hp);
			float x = startingX + j * (InvaderSprites[type].dim.x + InvadersColumnDistance);
			inv.obj->setPos(x, rowY);
			waveEnemies++;
		}
	}

}

Invaders::Invaders::Invader& Invaders::Invaders::spawnInvader(uint8_t type, uint8_t hitpoints){
	auto invaderObj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(InvaderSprites[type].path), InvaderSprites[type].dim),
			std::make_unique<RectCC>(InvaderSprites[type].dim)
	);
	addObject(invaderObj);
	std::weak_ptr<GameObject> weakPtr(invaderObj);
	if(playerBullet){
		const uint8_t hp = hitpoints;
		collision.addPair(*playerBullet, *invaderObj, [this, weakPtr, hp, type](){
			removeObject(playerBullet);
			playerBullet.reset();
			auto it = std::find_if(invaders.begin(), invaders.end(), [weakPtr](const Invader& inv){
				return inv.obj == weakPtr.lock();
			});
			auto invObj = weakPtr.lock();
			it->hitpoints--;
			if(it->hitpoints == 0){
				removeObject(invObj);
				invaders.erase(it);
				invaders.shrink_to_fit();
				score += hp;
				scoreDisplay->setScore(score);
				audio.play(InvaderDeathSounds[type]);

				setAchiIfBigger(Achievement::Resistron_b, score);
				setAchiIfBigger(Achievement::Resistron_g, score);
				setAchiIfBigger(Achievement::Resistron_s, score);
			}else{
				audio.play({ { 80, 80, 80 } });
			}
		});
	}

	collision.addPair(*killWall, *invaderObj, [this](){
		player->death();
		audio.play({ { 300, 400, 100 },
					 { 400, 300, 100 },
					 { 200, 300, 100 },
					 { 300, 200, 100 },
					 { 80,  80,  300 } });
	});

	invaders.emplace_back(invaderObj, hitpoints, hitpoints, type);
	return invaders.back();
}

void Invaders::Invaders::moveInvaders(float delta){
	const float speed = InvadersBaseSpeed * ((float) waveEnemies / (float) invaders.size());
	const float downSpeed = InvadersDownSpeed * ((float) waveEnemies / (float) invaders.size());
	if(invadersDirection == MoveDirection::Left){
		bool allLeft = false;

		for(auto& inv : invaders){
			if(inv.obj->getPos().x <= 1){
				allLeft = true;
				break;
			}
		}

		if(allLeft){
			nextDirection = MoveDirection::Right;
			invadersDirection = MoveDirection::Down;
			downCounter = 0;
			return;
		}

		for(auto& inv : invaders){
			inv.obj->setPos(inv.obj->getPos().x - delta * speed, inv.obj->getPos().y);
		}
	}else if(invadersDirection == MoveDirection::Right){
		bool allRight = false;
		for(auto& inv : invaders){
			if(inv.obj->getPos().x >= 128 - InvaderSprites[inv.type].dim.x - 1){
				allRight = true;
				break;
			}
		}

		if(allRight){
			nextDirection = MoveDirection::Left;
			invadersDirection = MoveDirection::Down;
			downCounter = 0;
			return;
		}

		for(auto& inv : invaders){
			inv.obj->setPos(inv.obj->getPos().x + delta * speed, inv.obj->getPos().y);
		}
	}else if(invadersDirection == MoveDirection::Down){
		bool allOnScreen = true;
		for(auto& inv : invaders){
			if(inv.obj->getPos().y < 0){
				allOnScreen = false;
				break;
			}
		}

		if(!allOnScreen){
			downCounter = 0;
		}else{
			downCounter += delta * downSpeed;
		}

		for(auto& inv : invaders){
			inv.obj->setPos(inv.obj->getPos().x, inv.obj->getPos().y + delta * downSpeed);
		}

		if(downCounter >= InvadersDownStep){
			invadersDirection = nextDirection;
		}
	}
}