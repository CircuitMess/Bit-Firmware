#ifndef BIT_FIRMWARE_HIGHSCOREMANAGER_H
#define BIT_FIRMWARE_HIGHSCOREMANAGER_H

#include <array>
#include <map>
#include "GameManager.h"

struct HighScore {
	char id[3] = {' ', ' ', ' '};
	uint32_t score = 0;
};

class HighScoreManager {
public:
	HighScoreManager();
	bool isHighScore(Games game, uint32_t score) const;
	void saveScore(Games game, HighScore score);
	const std::array<HighScore, 5>& getAll(Games game) const;
	bool hasHighScore(Games game) const;
	bool hasScore(Games game) const;

private:
	std::array<std::array<HighScore, 5>, (size_t)Games::COUNT> highScores;

private:
	static void setHighScores(Games game, const std::array<HighScore, 5>& scores) ;
	static bool getHighScores(Games game, std::array<HighScore, 5>& scores) ;
};

#endif //BIT_FIRMWARE_HIGHSCOREMANAGER_H