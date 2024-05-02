#ifndef BIT_FIRMWARE_INVADERS_H
#define BIT_FIRMWARE_INVADERS_H

#include "GameEngine/Game.h"
#include "Player.h"
#include "Games/Common/Hearts.h"
#include "Games/Common/Score.h"
#include "RoboCtrl/Resistron.h"

namespace Invaders {

class Invaders : public Game {
	friend Player;
public:
	Invaders(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onStop() override;

private:
	GameObjPtr bg;
	GameObjPtr playerObj;
	std::unique_ptr<Player> player;
	GameObjPtr playerBullet;
	GameObjPtr enemyBullet;
	std::unique_ptr<Hearts> hearts;
	std::unique_ptr<Score> scoreDisplay;

	GameObjPtr topWall;
	GameObjPtr bottomWall;
	GameObjPtr killWall;

	uint8_t lives = 3;
	uint32_t score = 0;
	uint32_t waveCounter = 0;
	uint8_t waveEnemies = 0; //num of enemies spawned in current wave


	static constexpr PixelDim PlayerSize = { 28, 17 };
	static constexpr PixelDim PlayerObjOffset = { 6, 12 };
	static constexpr PixelDim PlayerBulletDim = { 2, 5 };
	static constexpr PixelDim EnemyBulletDim = { 2, 4 };

	static constexpr float PlayerBulletSpeed = 150.0f;
	static constexpr float EnemyBulletSpeed = 100.0f;
	static constexpr float EnemyBaseShootPeriod = 4;
	float enemyShootTimer = 0;

	static constexpr uint8_t InvadersColumnDistance = 7; //distance between invaders on same row
	static constexpr uint8_t InvadersRowDistance = 2; //distance between rows
	static constexpr float InvadersBaseSpeed = 5.0;
	static constexpr float InvadersDownSpeed = 10.0;
	static constexpr float InvadersDownStep = InvadersRowDistance;
	struct InvaderSprite {
		const char* path;
		PixelDim dim;
	};
	static constexpr std::array<InvaderSprite, 4> InvaderSprites = {
			InvaderSprite{ "/invader1.raw", { 19, 21 } },
			InvaderSprite{ "/invader2.raw", { 22, 20 } },
			InvaderSprite{ "/invader3.raw", { 18, 24 } },
			InvaderSprite{ "/invader4.raw", { 18, 18 } }
	};
	static constexpr float EndPause = 2.5;
	float endCounter = 0;

	struct Invader {
		GameObjPtr obj;
		uint8_t hitpoints;
		uint8_t startingHP;
		uint8_t type; //0-3
	};
	std::vector<Invader> invaders;

	void shoot();
	bool enemyShoot();
	void spawnWave();
	Invader& spawnInvader(uint8_t type, uint8_t hitpoints);
	enum class MoveDirection {
		Left, Right, Down
	};
	MoveDirection invadersDirection = MoveDirection::Down;
	MoveDirection nextDirection = MoveDirection::Left;
	float downCounter = 0;
	void moveInvaders(float delta); //move left to the edge, then down, then right to the edge, then down etc.

	virtual void exit() override;

	static const Sound InvaderDeathSounds[4];

	std::shared_ptr<RoboCtrl::Resistron> robot;
};

}

#endif //BIT_FIRMWARE_INVADERS_H
