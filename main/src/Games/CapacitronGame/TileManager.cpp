#include "TileManager.h"
#include "GameEngine/Rendering/SpriteRC.h"
#include "GameEngine/Collision/RectCC.h"
#include <map>
#include <set>
#include "CapacitronGame.h"

CapacitronGame::TileManager::TileManager(std::vector<GameObjPtr>& tileObjs, std::vector<std::set<GameObjPtr>>& padObjs) : tileObjs(tileObjs), padObjs(padObjs){
	srand(time(nullptr));
}

void CapacitronGame::TileManager::addFiles(FileList bgFiles, FileList wallLFiles, FileList wallRFiles, FileList padFiles){
	this->bgFiles = bgFiles;
	this->wallLFiles = wallLFiles;
	this->wallRFiles = wallRFiles;
	this->padFiles = padFiles;
}

void CapacitronGame::TileManager::reset(uint8_t segmentIndex){
	drawTiles(segmentIndex);
	auto pos = tileObjs[segmentIndex]->getPos();
	pos.y -= (TilesPerSegment + 1) * BgTileDim;
	tileObjs[segmentIndex]->setPos(pos);
}

void CapacitronGame::TileManager::createBg(){
	for(int segID = 0; segID < SegmentCount; segID++){
		auto setOfTiles = std::make_shared<GameObject>(
				std::make_unique<SpriteRC>(PixelDim{ 128, BgTileDim }),
				std::make_unique<RectCC>(glm::vec2{ 128, BgTileDim })
		);
		tileObjs.push_back(setOfTiles);
		drawTiles(segID);
		setOfTiles->setPos({ 0, (segID - 1) * BgTileDim });
		setOfTiles->getRenderComponent()->setLayer(BgRenderLayer);
	}
}

void CapacitronGame::TileManager::createPads(float surface){
	const uint8_t surfaceTiles = (uint8_t) (surface * (float) PadTilesPerLevel);
	uint8_t tilesRequired = surfaceTiles;
	std::map<uint8_t, uint8_t> padsPerSize; //key is size of pads, value is number of pads of that size
	uint8_t numberOfPads = 0;
	while(tilesRequired > 0){
		uint8_t numTiles;
		if(tilesRequired <= 3){
			numTiles = tilesRequired;
		}else{
			numTiles = 2 + (rand() % (tilesRequired - 3)); //numTiles range from 2 to (tilesRequired-2), to avoid 1 tile long pads
		}

		if(!padsPerSize.contains(numTiles)){
			padsPerSize[numTiles] = 0;
		}
		padsPerSize[numTiles]++;
		tilesRequired -= numTiles;
		numberOfPads++;
	}


	uint8_t emptySpaces = PadTilesPerLevel - surfaceTiles;
	uint8_t tilesPlaced = 0;
	//get yPos relative to previously placed pads
	float yPos = 128;
	if(padObjs.empty()){
		yPos = 128 - PadTileDim;
	}else{
		//iterate over padObjs and find lowest getPos().y
		for(const auto& pads : padObjs){
			if(pads.empty()) continue;
			for(const auto& padObj : pads){
				if(padObj->getPos().y < yPos){
					yPos = padObj->getPos().y;
				}
			}
		}
		yPos -= CapacitronGame::JumpY;
	}

	std::set<GameObjPtr> pads;
	while(tilesPlaced < PadTilesPerLevel){
		//choose between empty spaces and pads
		int randNum = rand() % (emptySpaces + numberOfPads);
		if(randNum < emptySpaces){
			emptySpaces--;
			tilesPlaced++;
		}else{
			randNum -= emptySpaces;
			//go through map to select a pad
			int padCounter = 0;
			uint8_t selectedPadSize = 0;
			for(const auto& pair : padsPerSize){
				padCounter += pair.second;
				if(randNum < padCounter){
					selectedPadSize = pair.first;
					break;
				}
			}

			//construct pad object and place according to tilesPlaced
			auto padObj = std::make_shared<GameObject>(
					std::make_unique<SpriteRC>(PixelDim{ PadTileDim * selectedPadSize, PadTileDim }),
					std::make_unique<RectCC>(glm::vec2{ PadTileDim * selectedPadSize, PadTileDim })
			);
			pads.insert(padObj);
			padObj->setPos(WallTileDim + tilesPlaced * PadTileDim, yPos);
			drawPad(padObj, selectedPadSize);
			padObj->getRenderComponent()->setLayer(PadsRenderLayer);

			tilesPlaced += selectedPadSize;
			numberOfPads--;
			padsPerSize[selectedPadSize]--;
		}
	}
	padObjs.push_back(std::move(pads));
}

void CapacitronGame::TileManager::drawTiles(int objectIndex){
	auto sprite = std::static_pointer_cast<SpriteRC>(tileObjs[objectIndex]->getRenderComponent())->getSprite();

	for(int iTile = 0; iTile < TilesPerSegment; iTile++){
		int randBg = getRandomWallIndex();
		Display::drawFile(*sprite, bgFiles[randBg], iTile * BgTileDim, 0, BgTileDim, BgTileDim);
	}

	for(int iTile = 0; iTile < BgTileDim / WallTileDim; iTile++){
		int randWallL = rand() % wallLFiles.size();
		int randWallR = rand() % wallRFiles.size();
		Display::drawFile(*sprite, wallLFiles[randWallL], 0, iTile * WallTileDim, WallTileDim, WallTileDim);
		Display::drawFile(*sprite, wallRFiles[randWallR], 128 - WallTileDim, iTile * WallTileDim, WallTileDim, WallTileDim);
	}
}

void CapacitronGame::TileManager::drawPad(GameObjPtr padObj, uint8_t padSize){
	if(padSize == 0 || padSize >= PadTilesPerLevel){
		return;
	}
	const uint8_t startX = padObj->getPos().x;
	auto sprite = std::static_pointer_cast<SpriteRC>(padObj->getRenderComponent())->getSprite();
	const auto& leftTile = padFiles[padFiles.size() - 2];
	const auto& rightTile = padFiles[padFiles.size() - 1];

	sprite->clear(TFT_TRANSPARENT);

	//check for specific left tile
	uint8_t iTile = 0;
	if(startX != WallTileDim){
		Display::drawFile(*sprite, leftTile, iTile * PadTileDim, 0, PadTileDim, PadTileDim);
		iTile++;
	}
	for(; iTile < padSize - 1; ++iTile){
		uint8_t i = rand() % (padFiles.size() - 2);
		Display::drawFile(*sprite, padFiles[i], iTile * PadTileDim, 0, PadTileDim, PadTileDim);
	}

	//check for specific right tile
	if(startX + padSize * PadTileDim < 128 - WallTileDim){
		Display::drawFile(*sprite, rightTile, iTile * PadTileDim, 0, PadTileDim, PadTileDim);
		iTile++;
	}else{
		uint8_t i = rand() % (padFiles.size() - 2);
		Display::drawFile(*sprite, padFiles[i], iTile * PadTileDim, 0, PadTileDim, PadTileDim);
	}
}

uint8_t CapacitronGame::TileManager::getRandomWallIndex(){
	//30% probability for tiles 0-2, 10% for tile 3
	uint8_t index;
	uint8_t i = rand() % 101;
	if(i < 90){
		index = i / 30;
	}else{
		index = 3;
	}
	return index;
}




