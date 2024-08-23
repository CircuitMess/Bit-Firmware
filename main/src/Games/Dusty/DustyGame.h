#ifndef BIT_FIRMWARE_DUSTYGAME_H
#define BIT_FIRMWARE_DUSTYGAME_H

#include "GameEngine/Game.h"
#include "../Charlie/Storage.h"
#include "Games/Common/Score.h"
#include "Games/Common/Hearts.h"

namespace DustyGame {

class DustyGame : public Game {
public:
	DustyGame(Sprite& canvas);
	~DustyGame() override;

protected:
	uint32_t getXP() const override;

	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;
	void onRender(Sprite& canvas) override;
	void onStart() override;

private:
	struct {
		const char* path;
		PixelDim size;
	} static constexpr ItemsData[] = {
			{ "/item1.raw", { 23, 12 }},
			{ "/item2.raw", { 13, 14 }},
			{ "/item3.raw", { 8, 12 }},
			{ "/item4.raw", { 12, 9 }},
			{ "/item5.raw", { 26, 10 }},
			{ "/item6.raw", { 24, 21 }},
	};
	static constexpr auto ItemCount = sizeof(ItemsData) / sizeof(ItemsData[0]);

	static constexpr glm::vec2 CharPos = { 44, 5 };
	static constexpr glm::vec2 ArmPos = CharPos + glm::vec2 { 29, 12 } + glm::vec2 { 0, -14 };

	GameObjPtr armGo;

	int score = 0;
	int level = 0;
	int lives = 3;

	std::unique_ptr<Score> scoreEl;
	std::unique_ptr<Hearts> livesEl;

	static std::vector<glm::vec2> randPoints(size_t count);

	float spawnT = 0;
	bool spawning = false;
	void spawnItems();
	void updateSpawn(float dt);

	static constexpr uint8_t MaxItems = 6;
	struct Item {
		GameObjPtr go;
		int id;
	};
	CharlieGame::Storage<Item, MaxItems> items;

	enum {
		Aiming, Shooting, Retracting
	} state = Aiming;

	float swingT = 0;
	float shootT = 0;

	void updateAim(float dt);
	void updateShoot(float dt);
	void updateRetract(float dt);

	void shoot();
	float shootAngl;
	glm::vec2 shootDir;

	void wallCollision();

	struct CaughtItem {
		Item* item = nullptr;
		glm::vec2 offset = { 0, 0 };
		explicit operator bool() const { return item != nullptr; }
	} caught;
	void remCaught();
	void itemCollision(Item* item);

	static constexpr uint8_t MaxRats = 6;
	static constexpr float RatSpawnDelay = 6.0f;
	struct Rat {
		GameObjPtr go;
		float speed;
		float t = 0.0f;
	};
	CharlieGame::Storage<Rat, MaxRats> rats;
	float ratSpawnT = 0;

	void updateRats(float dt);
	void spawnRat();

	void ratHitItem(Rat* rat, Item* item);
	void ratHitArm(Rat* rat);

	struct {
		Rat* rat = nullptr;
		glm::vec2 offset;
		float startT = 0;
		glm::vec2 ropeStartOffset = { 0, 0 };
		glm::vec2 ropeSpd = { 0, 1 };
		explicit operator bool() const { return rat != nullptr; }
	} ratArm;

	struct RatItem {
		Rat* rat;
		CaughtItem* item;
	};
	CharlieGame::Storage<RatItem, MaxRats> ratItems;

};

}

#endif //BIT_FIRMWARE_DUSTYGAME_H
