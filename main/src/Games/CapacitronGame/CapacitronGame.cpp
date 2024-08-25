#include "CapacitronGame.h"
#include "Devices/Display.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Rendering/MultiRC.h"
#include "GameEngine/Collision/PolygonCC.h"
#include "Services/HighScoreManager.h"
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/Game/AwardsScreen.h"
#include <esp_random.h>

CapacitronGame::CapacitronGame::CapacitronGame(Sprite& canvas) : Game(canvas, Games::Capacitron, "/Games/Capacitron", {
		{ "/bg1.raw", {}, true },
		{ "/bg2.raw", {}, true },
		{ "/bg3.raw", {}, true },
		{ "/bg4.raw", {}, true },
		{ "/wallL1.raw", {}, true },
		{ "/wallL2.raw", {}, true },
		{ "/wallL3.raw", {}, true },
		{ "/wallR1.raw", {}, true },
		{ "/wallR2.raw", {}, true },
		{ "/wallR3.raw", {}, true },
		{ "/pad1.raw", {}, true },
		{ "/pad2.raw", {}, true },
		{ "/pad3.raw", {}, true },
		{ "/pad4.raw", {}, true },
		{ "/padL.raw", {}, true },
		{ "/padR.raw", {}, true },

		{ "/dead.gif", {}, false },
		{ "/jump.gif", {}, true },
		{ "/jumpGlow.gif", {}, true },
		{ "/jumpPad.gif", {}, false },
		{ "/heart.gif", {}, true },
		{ "/fireball.gif", {}, true },
		{ "/potion.gif", {}, true },
		RES_HEART,
		RES_GOBLET
}){
	robot = std::make_shared<RoboCtrl::Capacitron>();
	setRobot(robot);
}

void CapacitronGame::CapacitronGame::onLoad(){
	///Tiles
	tileManager = std::make_unique<TileManager>(tileObjs, padObjs, powerupObjs);
	tileManager->addFiles({ getFile("/bg1.raw"), getFile("/bg2.raw"), getFile("/bg3.raw"), getFile("/bg4.raw") },
						  { getFile("/wallL1.raw"), getFile("/wallL2.raw"), getFile("/wallL3.raw") },
						  { getFile("/wallR1.raw"), getFile("/wallR2.raw"), getFile("/wallR3.raw") },
						  { getFile("/pad1.raw"), getFile("/pad2.raw"), getFile("/pad3.raw"), getFile("/pad4.raw"), getFile("/padL.raw"),
							getFile("/padR.raw") },
						  { getFile("/potion.gif"), getFile("/heart.gif"), getFile("/jumpPad.gif") }
	);
	tileManager->createBg(); // creates GameObjects into movingTiles
	tileManager->onPowerupSpawn([this](Powerup p){ powerupSpawned(p); });

	bottomTileWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 128, 30 })
	);
	bottomTileWall->setPos(0, 128 + 32); //One tile below bottom wall
	addObject(bottomTileWall);
	for(int i = 0; i < tileObjs.size(); i++){
		auto obj = tileObjs[i];
		addObject(obj);
		collision.addPair(*bottomTileWall, *obj, [this, i](){ tileManager->reset(i); });
	}
	std::initializer_list<glm::vec2> points = { { 13, 27 },
												{ 5,  27 },
												{ 2,  21 },
												{ 2,  5 },
												{ 5,  2 },
												{ 10, 4 },
												{ 12, 18 } };
	playerObj = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/jump.gif")),
			std::make_unique<PolygonCC>(points)
	);
	playerObj->setPos({ (128 - PlayerSize.x) / 2, 128 - 8 - PlayerSize.y }); //spawn player in the middle of the first platform
	addObject(playerObj);

	playerLegsObj = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ PlayerLegsHitboxWidth, 1 }, glm::vec2{ (PlayerSize.x - PlayerLegsHitboxWidth) / 2, 0 })
	);
	playerLegsObj->setPos(playerObj->getPos() + glm::vec2{ 0, PlayerSize.y - 3 }); //spawn player in the middle of the first platform
	addObject(playerLegsObj);
	//TODO - make legs hitbox flip on left/right direction change,
	//	for this you will need to apply flipping as a GameObject attribute and apply it to CollisionComponents as well as RenderComponents
	player = std::make_unique<Player>(playerObj, playerLegsObj, this, getFile("/jump.gif"), getFile("/dead.gif"), getFile("/jumpGlow.gif"));

	collision.wallBot(*playerLegsObj, [this](){
		if(player->isDead()) return;

		if(player->isInvincible()){
			player->trampolineJump();
			return;
		}

		hearts->setLives(--lives);
		robot->flashingContinuous(-1);

		if(lives <= 0){
			player->death();
			audio.play({ { 400, 300, 200 },
						 { 0,   0,   50 },
						 { 300, 200, 200 },
						 { 0,   0,   50 },
						 { 200, 50,  400 } });
		}else{
			audio.play({ { 200, 50, 50 },
						 { 200, 50, 50 } });
			player->fallDown();
		}
	});

	hearts = std::make_unique<Hearts>(getFile(FILE_HEART));
	hearts->getGO()->setPos({ 2, 2 });
	addObject(hearts->getGO());
	hearts->setLives(lives);

	scoreDisplay = std::make_unique<Score>(getFile(FILE_GOBLET));
	scoreDisplay->getGO()->setPos({ 128 - 2 - 28, 2 });
	addObject(scoreDisplay->getGO());
	scoreDisplay->setScore(score);

	originalFireball = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/fireball.gif")),
			nullptr
	);
	originalFireball->setPos(-50, -50); //positioned out of screen, used only for its RenderComponent
	addObject(originalFireball);
	fireballAnimRC = std::static_pointer_cast<AnimRC>(originalFireball->getRenderComponent());
	fireballAnimRC->setLoopMode(GIF::Infinite);
	fireballAnimRC->start();

	createPad(1, false, 0);
	createPad(0.75, false, 0);
	createPad(0.75, false, 0);
	createPad(0.75, false, 0);
	createPad(0.75, false, 0);
	createPad(0.75, false, 0);

	robot->leftRightContinuous(1000);

}

void CapacitronGame::CapacitronGame::onLoop(float deltaTime){
	float yShift = player->update(deltaTime);

	if(playerObj->getPos().y >= 150){
		exit();
		return;
	}

	fireballTimer += deltaTime;
	if(fireballTimer >= fireballInterval){
		fireballTimer = 0;
		spawnFireball();
		fireballInterval = FireballStartingInterval - ((float) score / MaxDifficultyScore) * (FireballStartingInterval - FireballMinimumInterval);
	}

	for(const auto& obj : fireballObjs){
		auto pos = obj->getPos();
		pos.y += deltaTime * FireballSpeed;
		obj->setPos(pos);
	}


	if(!cameraShifting) return;
	totalShift += abs(yShift);
	if(yShift >= 0){
		cleanupPads();
		cameraShifting = false;
		totalShift = 0;
		return;
	}

	if(totalShift >= camShiftDistance){
		cleanupPads();
		cameraShifting = false;
		yShift = -(totalShift - camShiftDistance);
		totalShift = 0;
	}

	for(const auto& obj : tileObjs){
		auto pos = obj->getPos();
		pos.y -= yShift;
		obj->setPos(pos);
	}

	for(const auto& pads : padObjs){
		for(const auto& obj : pads){
			auto pos = obj->getPos();
			pos.y -= yShift;
			obj->setPos(pos);
		}
	}

	for(const auto& obj : powerupObjs){
		auto pos = obj->getPos();
		pos.y -= yShift;
		obj->setPos(pos);
	}

	for(const auto& obj : fireballObjs){
		auto pos = obj->getPos();
		pos.y -= yShift;
		obj->setPos(pos);
	}

	auto playerPos = playerObj->getPos();
	playerObj->setPos(playerPos - glm::vec2{ 0, yShift });
}

void CapacitronGame::CapacitronGame::handleInput(const Input::Data& data){
	if(data.action == Input::Data::Press){
		player->btnPressed(data.btn);
	}else if(data.action == Input::Data::Release){
		player->btnReleased(data.btn);
	}
}

void CapacitronGame::CapacitronGame::onStop(){
	player->btnReleased(Input::Left);
	player->btnReleased(Input::Right);
}

uint32_t CapacitronGame::CapacitronGame::getXP() const{
	return score * 8;
}

void CapacitronGame::CapacitronGame::createPad(float surface, bool powerupsEnabled, uint8_t powerupRate){
	tileManager->createPads(surface, powerupsEnabled, powerupRate);
	for(const auto& obj : padObjs.back()){
		addObject(obj);
		collision.addPair(*obj, *playerLegsObj, [this](){
			if(player->isDead()) return;
			if(player->getYSpeed() < 0) return;

			player->jump();

			if(playerObj->getPos().y <= (*padObjs.front().begin())->getPos().y - JumpY - JumpYExtra){
				cameraShifting = true;
				camShiftDistance = 128 - 8 - (*padObjs[1].begin())->getPos().y;
				scoreDisplay->setScore(++score);

				setAchiIfBigger(Achievement::Capacitron_b, score);
				setAchiIfBigger(Achievement::Capacitron_s, score);
				setAchiIfBigger(Achievement::Capacitron_g, score);
			}
		});
	}
}

void CapacitronGame::CapacitronGame::powerupSpawned(Powerup powerup){
	addObject(powerup.obj);
	const auto type = powerup.type;
	std::weak_ptr<GameObject> weakPtr(powerup.obj);

	collision.addPair(*playerObj, *powerup.obj, [this, type, weakPtr](){
		if(player->isDead()) return;

		auto obj = weakPtr.lock();

		switch(type){
			case Powerup::Type::Potion:
				audio.play({ { 80, 800, 80 },
							 { 0,  0,   80 },
							 { 80, 800, 80 } });
				robot->allOn();

				removeObject(obj);
				powerupObjs.erase(std::remove(powerupObjs.begin(), powerupObjs.end(), obj));
				powerupObjs.shrink_to_fit();
				player->invincible();
				break;
			case Powerup::Type::HalfHeart:
				removeObject(obj);
				powerupObjs.erase(std::remove(powerupObjs.begin(), powerupObjs.end(), obj));
				powerupObjs.shrink_to_fit();
				audio.play({ { 80, 800, 80 } });
				if(halfHeartCollected && lives < 3){
					lives++;
					hearts->setLives(lives);
					halfHeartCollected = false;
				}else{
					halfHeartCollected = true;
				}
				break;
			case Powerup::Type::Trampoline:
				break;
		}
	});

	collision.addPair(*playerLegsObj, *powerup.obj, [this, type, weakPtr](){
		if(player->isDead()) return;

		auto obj = weakPtr.lock();

		switch(type){
			case Powerup::Type::Potion:
				break;
			case Powerup::Type::HalfHeart:
				break;
			case Powerup::Type::Trampoline:
				if(player->getYSpeed() < 0) return;

				audio.play({ { 80, 900, 250 } });
				player->trampolineJump();
				auto anim = std::static_pointer_cast<AnimRC>(obj->getRenderComponent());
				anim->reset();
				anim->start();

				if(playerObj->getPos().y <= (*padObjs.front().begin())->getPos().y - JumpY - JumpYExtra){
					cameraShifting = true;
					camShiftDistance = 128 - 8 - (*padObjs[2].begin())->getPos().y;
					score += 2;
					scoreDisplay->setScore(score);
				}else{
					cameraShifting = true;
					camShiftDistance = 128 - 8 - (*padObjs[1].begin())->getPos().y;
					score += 2;
					scoreDisplay->setScore(score);
				}
				break;
		}
	});

	collision.addPair(*bottomTileWall, *powerup.obj, [this, weakPtr](){
		removeObject(powerupObjs.front());
		powerupObjs.erase(powerupObjs.begin());
		powerupObjs.shrink_to_fit();
	});
}

void CapacitronGame::CapacitronGame::spawnFireball(){
	auto fireball = std::make_shared<GameObject>(
			std::make_unique<MultiRC>(fireballAnimRC),
			std::make_unique<RectCC>(glm::vec2{ 13, 21 })
	);

	const float x = esp_random() % (128 - 13);
	fireball->setPos(x, -50);
	fireballObjs.push_back(fireball);
	addObject(fireball);

	collision.addPair(*bottomTileWall, *fireball, [this](){
		removeObject(fireballObjs.front());
		fireballObjs.erase(fireballObjs.begin());
		fireballObjs.shrink_to_fit();

	});
	std::weak_ptr<GameObject> weakPtr(fireball);
	collision.addPair(*fireball, *playerObj, [this, weakPtr](){
		auto obj = weakPtr.lock();
		if(player->isDead()) return;

		if(!player->isInvincible()){

			hearts->setLives(--lives);
			robot->flashingContinuous(-1);

			if(lives <= 0){
				player->death();
				audio.play({ { 400, 300, 200 },
							 { 0,   0,   50 },
							 { 300, 200, 200 },
							 { 0,   0,   50 },
							 { 200, 50,  400 } });
			}else{
				audio.play({ { 200, 50, 100 } });

				player->damage();
			}
		}else{
			addAchi(Achievement::Capacitron_fire, 1);
		}
		removeObject(obj);

	});
}

void CapacitronGame::CapacitronGame::cleanupPads(){
	const float surface = std::clamp(StartingSurface - ((float) score / MaxDifficultyScore) * (StartingSurface - MinimumSurface),
									 MinimumSurface, StartingSurface);
	while((**padObjs.front().begin()).getPos().y >= 128){

		for(const auto& obj : padObjs.front()){
			removeObject(obj);
		}
		padObjs.erase(padObjs.begin());
		padObjs.shrink_to_fit();
		const uint8_t rate = PowerupsStartingRate - (PowerupsStartingRate - PowerupsMinimumRate) * (score - PowerupsStartScore) / MaxDifficultyScore;
		createPad(surface, score > PowerupsStartScore, rate);
	}
}