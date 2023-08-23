#include "CapacitronGame.h"
#include "Devices/Display.h"
#include "GameEngine/Collision/RectCC.h"
#include "GameEngine/Rendering/SpriteRC.h"

CapacitronGame::CapacitronGame::CapacitronGame(Sprite& canvas) : Game(canvas, "/Games/Capacitron", {
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
		{ "/heart.gif", {}, false },
		{ "/fireball.gif", {}, false },
		{ "/potion.gif", {}, false },
		RES_HEART,
		RES_GOBLET
}){}

void CapacitronGame::CapacitronGame::onLoad(){
	///Tiles
	tileManager = std::make_unique<TileManager>(tileObjs, padObjs);
	tileManager->addFiles({ getFile("/bg1.raw"), getFile("/bg2.raw"), getFile("/bg3.raw"), getFile("/bg4.raw") },
						  { getFile("/wallL1.raw"), getFile("/wallL2.raw"), getFile("/wallL3.raw") },
						  { getFile("/wallR1.raw"), getFile("/wallR2.raw"), getFile("/wallR3.raw") },
						  { getFile("/pad1.raw"), getFile("/pad2.raw"), getFile("/pad3.raw"), getFile("/pad4.raw"), getFile("/padL.raw"), getFile("/padR.raw") }
	);
	tileManager->createBg(); // creates GameObjects into movingTiles

	bottomTileWall = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ 128, 1 })
	);
	bottomTileWall->setPos(0, 128 + 32); //One tile below bottom wall
	addObject(bottomTileWall);
	for(int i = 0; i < tileObjs.size(); i++){
		auto obj = tileObjs[i];
		addObject(obj);
		collision.addPair(*bottomTileWall, *obj, [this, i](){ tileManager->reset(i); });
	}

	playerObj = std::make_shared<GameObject>(
			std::make_unique<AnimRC>(getFile("/jump.gif")),
			std::make_unique<RectCC>(glm::vec2{ 20, 30 })
	);
	playerObj->setPos({ (128 - PlayerSize.x) / 2, 128 - 8 - PlayerSize.y }); //spawn player in the middle of the first platform
	addObject(playerObj);

	playerLegsObj = std::make_shared<GameObject>(
			nullptr,
			std::make_unique<RectCC>(glm::vec2{ PlayerLegsHitboxWidth, 1 }, glm::vec2{ (PlayerSize.x - PlayerLegsHitboxWidth) / 2, 0 })
	);
	playerLegsObj->setPos(playerObj->getPos() + glm::vec2{ 0, PlayerSize.y }); //spawn player in the middle of the first platform
	addObject(playerLegsObj);
	//TODO - make legs hitbox flip on left/right direction change,
	//	for this you will need to apply flipping as a GameObject attribute and apply it to CollisionComponents as well as RenderComponents
	player = std::make_unique<Player>(playerObj, playerLegsObj, this, getFile("/jump.gif"), getFile("/dead.gif"));

	collision.wallBot(*playerLegsObj, [this](){
		if(player->isDead()) return;

		hearts->setLives(--lives);
		if(lives <= 0){
			player->death();
		}else{
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

	createPad(1);
	createPad(0.75);
	createPad(0.75);
	createPad(0.75);
}

void CapacitronGame::CapacitronGame::onLoop(float deltaTime){
	float yShift = player->update(deltaTime);

	if(!cameraShifting) return;
	totalShift += abs(yShift);
	if(yShift >= 0){
		cameraShifting = false;
		totalShift = 0;
		return;
	}

	if(totalShift >= camShiftDistance){
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

void CapacitronGame::CapacitronGame::createPad(float surface){
	tileManager->createPads(surface);
	for(const auto& obj : padObjs.back()){
		addObject(obj);
		collision.addPair(*obj, *playerLegsObj, [this](){
			if(player->getYSpeed() < 0) return;

			player->jump();

			if(playerObj->getPos().y <= (*padObjs.front().begin())->getPos().y - JumpY - JumpYExtra){
				cameraShifting = true;
				camShiftDistance = 128 - 8 - (*padObjs[1].begin())->getPos().y;
				scoreDisplay->setScore(++score);
			}
		});
	}

	//collider for creating a new pad is applied to only 1 pad in the whole level
	collision.addPair(*bottomTileWall, **padObjs.front().begin(), [this](){
		for(const auto& obj : padObjs.front()){
			removeObject(obj);
		}
		padObjs.erase(padObjs.begin());
		padObjs.shrink_to_fit();

		createPad(0.25); //TODO - add difficulty ramp up
	});
}
