#ifndef BIT_FIRMWARE_CHARLIEGAME_H
#define BIT_FIRMWARE_CHARLIEGAME_H


#include "GameEngine/Game.h"
#include "Char.h"
#include "Fly.h"
#include "Games/Common/Score.h"
#include "Games/Common/Hearts.h"
#include "Storage.h"

namespace CharlieGame {

class CharlieGame : public Game {
protected:
public:
	CharlieGame(Sprite& base);
	~CharlieGame() override;

private:
	uint32_t getXP() const override;
protected:
	uint32_t getScore() const override;
private:

	void onLoad() override;
	void handleInput(const Input::Data& data) override;
	void onLoop(float deltaTime) override;

private:
	int score = 0;
	int lives = 3;

	std::unique_ptr<Score> scoreEl;
	std::unique_ptr<Hearts> livesEl;

	std::unique_ptr<Char> chrl;

	static constexpr size_t MaxFlies = 8;
	Storage<Fly, MaxFlies> flies;

	static constexpr float FlySpawnRate = 4.0f;
	float flySpawnT = 3.0f;
	void updateFlies(float dt);

	bool rolling = false;
	float rollT = 0;
	Fly* rollingFly;
	static constexpr float RollTime = 2.0f;
	void startRoll();
	void stopRoll();
	void updateRoll(float dt);

	Storage<Cacoon, MaxFlies> cacs;
	static constexpr float CacoonTime = 4.0f;
	void updateCacs(float dt);

	struct Puf {
		GameObjPtr go;
		float t;
	};
	std::vector<Puf> pufs;
	void updatePufs(float dt);
	static constexpr float PufDuration = 0.56f;

	void dmg();
	bool over = false;
	float overT = 0;
	static constexpr float OverTimeout = 2.0f;
	void gameOver();
	void updateOver(float dt);

	int cacoons = 0;
	int escapes = 0;

};

};

#endif //BIT_FIRMWARE_CHARLIEGAME_H
