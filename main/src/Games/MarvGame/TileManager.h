#ifndef BIT_FIRMWARE_TILEMANAGER_H
#define BIT_FIRMWARE_TILEMANAGER_H

#include <memory>
#include <vector>
#include "GameEngine/GameObject.h"

class TileManager {
public:
	TileManager(std::vector<std::shared_ptr<GameObject>>& movingObjects);
	void addFile(File topFile);
	void reset(int objectIndex);
	void create();

private:
	void drawTiles(int objectIndex);

	static constexpr PixelDim tileDim = { 8, 7 };
	const int tilesPerArray = 10;
	const int topY = 88;
	const int arrays = 3;

	std::vector<File> topFiles;
	std::vector<std::shared_ptr<GameObject>>& movingObjects;
};


#endif //BIT_FIRMWARE_TILEMANAGER_H
