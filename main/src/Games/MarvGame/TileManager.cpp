#include "TileManager.h"
#include "../../GameEngine/Rendering/SpriteRC.h"
#include "../../GameEngine/Collision/RectCC.h"

TileManager::TileManager(std::vector<std::shared_ptr<GameObject>>& movingObjects) : movingObjects(movingObjects){
}

void TileManager::addFile(File topFile){
	topFiles.push_back(topFile);
}

void TileManager::create(){
	for(int iArray = 0; iArray < arrays; iArray++){

		auto setOfTiles = std::make_shared<GameObject>(
				std::make_unique<SpriteRC>(PixelDim{ tileDim.x * tilesPerArray, tileDim.y }),
				std::make_unique<RectCC>(tileDim)
		);
		movingObjects.push_back(setOfTiles);
		drawTiles(iArray);
		setOfTiles->setPos({ iArray * tilesPerArray * tileDim.x, topY });
		setOfTiles->getRenderComponent()->setLayer(-1);
	}

}

void TileManager::reset(int objectIndex){
	int maxX = 0;
	for(auto& obj : movingObjects){
		if(obj->getPos().x > maxX){
			maxX = obj->getPos().x;
		}
	}
	movingObjects[objectIndex]->setPos({ maxX + tilesPerArray * tileDim.x, topY });
}

void TileManager::drawTiles(int objectIndex){
	auto sprite = std::static_pointer_cast<SpriteRC>(movingObjects[objectIndex]->getRenderComponent())->getSprite();

	sprite->clear(TFT_TRANSPARENT);
	for(int iTile = 0; iTile < tilesPerArray; iTile++){
		int randTop = rand() % topFiles.size();
		Display::drawFile(*sprite, topFiles[randTop], iTile * tileDim.x, 0, tileDim.x, tileDim.y);
	}
}
