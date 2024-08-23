#ifndef BIT_FIRMWARE_HARALD_H
#define BIT_FIRMWARE_HARALD_H

#include "GameEngine/Game.h"
#include "Score.h"

namespace Harald {
class Harald : public Game {
public:
	explicit Harald(Sprite& canvas);

protected:
	void onLoad() override;
	void onLoop(float deltaTime) override;
	void handleInput(const Input::Data& data) override;

	inline uint32_t getXP() const override { return score / 12; };
	inline uint32_t getScore() const override { return score; }

private:
	struct GameElement {
		uint8_t id = 0; // 0 - 10
		GameObjPtr gameObj;
	};

	using Field = std::array<std::array<GameElement, 4>, 4>;
	using IdField = std::array<std::array<uint8_t, 4>, 4>;

	Field elements;
	uint32_t score = 0;
	std::unique_ptr<Score> scoreElement;
	std::unique_ptr<Score> bestScoreElement;

	std::vector<GameObjPtr> bgTiles;

	inline static constexpr const char* Icons[] = {
			"/Tile01.raw",
			"/Tile02.raw",
			"/Tile03.raw",
			"/Tile04.raw",
			"/Tile05.raw",
			"/Tile06.raw",
			"/Tile07.raw",
			"/Tile08.raw",
			"/Tile09.raw",
			"/Tile10.raw",
			"/Tile11.raw",
	};

	struct TileMove {
		glm::vec<2, uint8_t> source;
		glm::vec<2, uint8_t> target;
		bool combo;
	};

	struct TileMoveComp { bool operator()(const TileMove& a, const TileMove& b) const {
		return ((int) a.source.x * 3 + (int) a.source.y * 7) < ((int) b.source.x * 3 + (int) b.source.y * 7);
	} };
	std::set<TileMove, TileMoveComp> tileMoves;
	struct {
		IdField field;
		uint32_t score;
	} moveResult;
	float tileMoveT = 0;
	static constexpr float TileMoveSpeed = 2.5f;

	struct Puf {
		GameObjPtr go;
		float time = 0;
	};
	std::vector<Puf> pufs;

	void findMoves(Input::Button dir);
	void applyMove();
	void spawnNew();

	void tileMoveAnim(float dt);

	static constexpr float PufDuration = 0.42f;
	void checkPufs(float dt);

	void checkLoseCondition();

	void gameWin();
	void gameLose();

	bool exiting = false;
	float exitT = 0;
	bool won = false;
	static constexpr float ExitPauseLose = 0.85f;
	static constexpr float ExitPauseWin = 0.75f;
	struct ExitObj {
		GameObjPtr go;
		glm::vec2 speed;
	};
	std::vector<ExitObj> exitObjs;
	GameObjPtr floor;
	void exitStart();
	void exitAnim(float dt);

};
} // Harald

#endif //BIT_FIRMWARE_HARALD_H