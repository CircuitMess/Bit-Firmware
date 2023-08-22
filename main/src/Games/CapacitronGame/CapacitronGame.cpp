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

		{ "/dead.raw", {}, false },
		{ "/jump.gif", {}, true },
		{ "/jumpGlow.gif", {}, true },
		{ "/jumpPad.gif", {}, false },
		{ "/heart.gif", {}, false },
		{ "/fireball.gif", {}, false },
		{ "/potion.gif", {}, false },
		RES_HEART
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

	createPad(0.75);
	createPad(0.75);
	createPad(0.75);
	createPad(0.75);
}

void CapacitronGame::CapacitronGame::onLoop(float deltaTime){

	for(const auto& obj : tileObjs){
		auto pos = obj->getPos();
		pos.y = pos.y + deltaTime * speed;
		obj->setPos(pos);
	}

	for(const auto& pads : padObjs){
		for(const auto& obj : pads){
			auto pos = obj->getPos();
			pos.y = pos.y + deltaTime * speed;
			obj->setPos(pos);
		}
	}
}

void CapacitronGame::CapacitronGame::handleInput(const Input::Data& data){
	Game::handleInput(data);
}

void CapacitronGame::CapacitronGame::createPad(float surface){
	tileManager->createPads(surface);
	for(const auto& obj : padObjs.back()){
		addObject(obj);
	}

	//collider for creating a new pad is set only to 1 pad in set
	collision.addPair(*bottomTileWall, **padObjs.front().begin(), [this](){
		for(const auto& obj : padObjs.front()){
			removeObject(obj);
		}
		padObjs.erase(padObjs.begin());
		padObjs.shrink_to_fit();

		createPad(0.75);
	});
}
