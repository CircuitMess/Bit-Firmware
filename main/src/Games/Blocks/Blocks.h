#ifndef BIT_FIRMWARE_BLOCKS_H
#define BIT_FIRMWARE_BLOCKS_H

#include "GameEngine/Game.h"
#include "Block.h"
#include "GameEngine/Rendering/TextRC.h"

class Blocks : public Game {
	friend Block;
public:
	Blocks(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;

private:
	enum class State {
		Running, GameOver, GameWin
	} state = State::Running;

	std::vector<Block> blocks;
	GameObjPtr bg;
	GameObjPtr scoreLabel;
	GameObjPtr scoreNumLabel;
	std::shared_ptr<TextRC> scoreTextRC;

	GameObjPtr nextLabel;
	Block nextBlock;

	GameObjPtr levelLabel;
	std::shared_ptr<TextRC> levelTextRC;

	GameObjPtr linesLabel;
	GameObjPtr linesNumLabel;
	std::shared_ptr<TextRC> linesTextRC;


	uint8_t level = 0;
	uint32_t linesCleared = 0;
	uint32_t score = 0;
	float moveBuffer = 0; //this accumulates to TileDim over time, until the position changes
	float popCounter = 0;

	static constexpr PixelDim TileDim = { 6, 6 };
	static constexpr PixelDim GridDim = { 10, 24 };
	//grid starts outside screen for vanish zone (rows 21-24)
	static constexpr PixelDim GridPos = { (128 - GridDim.x * TileDim.x) / 2, (128 - GridDim.y * TileDim.y) };
	static constexpr uint8_t KillHeight = 21; //if a block is placed in row 21, that's game over
	static constexpr PixelDim NextBlockPos = { -3, 16 };

	static constexpr uint8_t MaxLevel = 8;
	static constexpr float BaseSpeed = TileDim.y;
	static constexpr float SoftDropSpeed = 30 * TileDim.y;
	static constexpr float LevelSpeedFactors[MaxLevel + 1] = { 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3 };
	static constexpr float MaxLevelSpeedFactorIncrement = 0.25;
	static constexpr uint32_t BlockTypesNum = 7;
	static constexpr uint32_t BlockColorsNum = 6;
	static constexpr const char* Sprites[BlockColorsNum + 1] = {
			"/red.raw", "/green.raw", "/yellow.raw", "/cyan.raw", "/gold.raw", "/purple.raw", "/placed.raw"
	};

//	static constexpr float AnimPeriod = 0.2;
//	static constexpr uint8_t AnimLoops = 3;
//	float animTimer = 0;
//	float animLoopCount = 0;
//	bool animState = false;
	//TODO - animations for line clears

	static constexpr uint8_t PopPause = 2;
	static constexpr float LongPressTime = 0.35;
	static constexpr float LongPressRepeatTime = 0.02;
	Input::Button lastButton = Input::Menu;
	enum {
		Pressed, Released, Held
	} lastButtonState = Released;
	float btnHoldTimer = 0;
	bool fastDrop = false;

	bool blocksMatrix[GridDim.x][GridDim.y];


	void newBlock();
	void gameOver();
	bool moveBlock(bool fastDrop);
	void handleInputRepeat(float deltaT);
	void sendLastInput();
	bool checkBoundLeft(const Block& block);
	bool checkBoundRight(const Block& block);
	void checkLineClear();
	void clearSegment(Block& block, GameObjPtr segment);
	void updateScore();

	static constexpr PixelDim globalToGridPos(PixelDim globalPos){
		return (globalPos - Blocks::GridPos) / Blocks::TileDim.x;
	}

	static constexpr PixelDim gridToGlobalPos(PixelDim gridPos){
		return gridPos * TileDim.x + GridPos;
	}
};


#endif //BIT_FIRMWARE_BLOCKS_H
