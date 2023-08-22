#ifndef BIT_FIRMWARE_CAP_TILEMANAGER_H
#define BIT_FIRMWARE_CAP_TILEMANAGER_H

#include <memory>
#include <vector>
#include <set>
#include <queue>
#include "GameEngine/GameObject.h"

typedef std::initializer_list<File> FileList;

namespace CapacitronGame {

class TileManager {
public:
	TileManager(std::vector<GameObjPtr>& tileObjs, std::vector<std::set<GameObjPtr>>& padObjs);
	void addFiles(FileList bgFiles, FileList wallLFiles, FileList wallRFiles, FileList padFiles);
	void reset(uint8_t segmentIndex);
	void createBg();

	/**
	 * Creates a set of pads on the next jump level
	 * @param surface Percentage of screen area that the pads on this jump level must have.
	 */
	void createPads(float surface);

private:
	void drawTiles(int objectIndex);
	void drawPad(GameObjPtr padObj, uint8_t padSize);

	static constexpr uint8_t BgTileDim = 32; //32x32 tiles
	static constexpr uint8_t WallTileDim = 8; //8x8 tiles
	static constexpr uint8_t PadTileDim = 8; //8x8 tiles
	static constexpr uint8_t SegmentCount = 5; //number of 128x32 segments
	static constexpr uint8_t TilesPerSegment = 4; //4 bg tiles per segment
	static constexpr uint8_t PadTilesPerLevel = (128 - WallTileDim * 2) / PadTileDim;
	static constexpr int PadsRenderLayer = -1;
	static constexpr int BgRenderLayer = -2;

	std::vector<File> bgFiles;
	std::vector<File> wallLFiles;
	std::vector<File> wallRFiles;
	std::vector<File> padFiles; //last 2 tiles are the fixed left and right tiles, respectively
	std::vector<GameObjPtr>& tileObjs;
	std::vector<std::set<GameObjPtr>>& padObjs;

	uint8_t getRandomWallIndex(); //takes into account distribution of random tiles
};

}
#endif //BIT_FIRMWARE_CAP_TILEMANAGER_H
