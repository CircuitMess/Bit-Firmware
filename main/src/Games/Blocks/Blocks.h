#ifndef BIT_FIRMWARE_BLOCKS_H
#define BIT_FIRMWARE_BLOCKS_H

#include "GameEngine/Game.h"
#include "Block.h"

class Blocks : public Game {
	friend Block;
public:
	Blocks(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;

private:
	enum class State {
		Running, GameOver, GameWin
	} state = State::Running;

	std::vector<Block> blocks;
	GameObjPtr bg;
	GameObjPtr scoreElement;

	uint8_t level = 0;
	uint32_t lines = 0;
	uint32_t score = 0;
	float moveBuffer = 0; //this accumulates to TileDim over time, until the position changes
	float popCounter = 0;

	static constexpr PixelDim TileDim = { 6, 6 };
	static constexpr PixelDim GridDim = { 10, 24 };
	//grid starts outside screen for vanish zone (rows 21-24)
	static constexpr PixelDim GridPos = { (128 - GridDim.x * TileDim.x) / 2, (128 - GridDim.y * TileDim.y) };
	static constexpr uint8_t KillHeight = 21; //if a block is placed in row 21, that's game over

	static constexpr uint8_t MaxLevel = 9;
	static constexpr float BaseSpeed = TileDim.y;
	static constexpr float SoftDropSpeed = 30 * TileDim.y;
	static constexpr float LevelSpeedFactors[MaxLevel] = { 1, 1.25, 1.5, 1.75, 2, 2.25, 2.5, 2.75, 3 };
	static constexpr uint32_t BlockTypesNum = 7;
	static constexpr uint32_t BlockColorsNum = 6;
	static constexpr Color colors[BlockColorsNum] = { TFT_RED, TFT_GREEN, TFT_YELLOW, TFT_CYAN, TFT_GOLD, TFT_PURPLE };

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
	bool moveBlock();
	void handleInputRepeat(float deltaT);
	void sendLastInput();
	bool checkBoundLeft(const Block& block);
	bool checkBoundRight(const Block& block);

	static constexpr PixelDim globalToGridPos(PixelDim globalPos){
		return (globalPos - Blocks::GridPos) / Blocks::TileDim.x;
	}
};


#endif //BIT_FIRMWARE_BLOCKS_H
