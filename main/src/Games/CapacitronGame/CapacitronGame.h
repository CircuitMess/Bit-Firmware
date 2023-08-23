#ifndef BIT_FIRMWARE_CAPACITRONGAME_H
#define BIT_FIRMWARE_CAPACITRONGAME_H

#include "GameEngine/Game.h"
#include "TileManager.h"
#include "Player.h"

namespace CapacitronGame {

class TileManager;

class CapacitronGame : public Game {
	friend TileManager;
	friend Player;
public:
	CapacitronGame(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;

private:
	std::unique_ptr<TileManager> tileManager;
	std::vector<GameObjPtr> tileObjs;
	std::vector<std::set<GameObjPtr>> padObjs; //pads are split in a queue of "levels", which are sets of 1 or more pad
	GameObjPtr bottomTileWall;
	GameObjPtr playerObj;
	GameObjPtr playerLegsObj;
	std::unique_ptr<Player> player;

	static constexpr float speed = 5;
	static constexpr float JumpY = 42; //distance covered by one jump
	static constexpr float Gravity = 150.0f;
	static constexpr float JumpYExtra = 20; //extra lee-way so that the player jumps up and over the next platform
	static constexpr float JumpSpeed = -Gravity + JumpY - JumpYExtra;
	static constexpr PixelDim PlayerSize = { 20, 30 };
	static constexpr uint8_t PlayerLegsHitboxWidth = 12;

	void createPad(float surface);
	float counter = 0;
	bool cameraShifting = false;
	float camShiftDistance = 0;
	float totalShift = 0;
};
/*
 * logika igre
 *
 * spawnanje padova:
 * x-os što dalja od prošlog pada kako difficulty raste
 * y-os uvijek u koracima od JumpY
 * veličina pada i broj padova na svakoj jump razini - randomizean broj predstavlja "površinu", koju ostvarujemo s proizvoljnim brojem/veličinom padova
 * ukupna "površina" padova je random, prva površina je uvijek maksimalna, ostale random sa minimumom koji pada kako difficulty raste
 *
 *
 */
}

#endif //BIT_FIRMWARE_CAPACITRONGAME_H
