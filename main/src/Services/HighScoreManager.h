#ifndef BIT_FIRMWARE_HIGHSCOREMANAGER_H
#define BIT_FIRMWARE_HIGHSCOREMANAGER_H

#include <array>
#include <map>
#include "RobotManager.h"

struct HighScore {
	char id[3] = {' ', ' ', ' '};
	uint32_t score = 0;
};

class HighScoreManager {
public:
	HighScoreManager();

	static constexpr uint8_t Count = 4;

	bool isHighScore(Games game, uint32_t score) const;
	void saveScore(Games game, HighScore score);
	const std::array<HighScore, Count>& getAll(Games game) const;
	bool hasHighScore(Games game) const;
	bool hasScore(Games game) const;


private:
	std::array<std::array<HighScore, Count>, (size_t)Games::COUNT> highScores;

private:
	static void setHighScores(Games game, const std::array<HighScore, Count>& scores) ;
	static bool getHighScores(Games game, std::array<HighScore, Count>& scores) ;
};

#endif //BIT_FIRMWARE_HIGHSCOREMANAGER_H