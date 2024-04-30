#include "HighScoreManager.h"
#include "Periph/NVSFlash.h"
#include "Util/Services.h"
#include <algorithm>

HighScoreManager::HighScoreManager(){
	for(size_t i = 0; i < (size_t) Games::COUNT; ++i){
		getHighScores((Games) i, highScores[i]);
		std::sort(highScores[i].begin(), highScores[i].end(), [](const HighScore& first, const HighScore& second) {
			return (first.valid && !second.valid) || first.score > second.score;
		});
	}
}

bool HighScoreManager::isHighScore(Games game, uint32_t score) const{
	for(const HighScore& highScore : highScores[(size_t) game]){
		if(highScore.score == score){
			return false;
		}
	}

	return !highScores[(size_t) game].back().valid || highScores[(size_t) game].back().score < score;
}

void HighScoreManager::saveScore(Games game, HighScore score){
	highScores[(size_t) game].back() = highScores;
	std::sort(highScores[(size_t) game].begin(), highScores[(size_t) game].end(), [](const HighScore& first, const HighScore& second) {
		return (first.valid && !second.valid) || first.score > second.score;
	});
	setHighScores(game, highScores[(size_t) game]);
}

const std::array<HighScore, 5>& HighScoreManager::getAll(Games game) const{
	return highScores[(size_t) game];
}

void HighScoreManager::setHighScores(Games game, const std::array<HighScore, 5>& score) const{
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	const std::string blob = std::string("HighScore") + std::to_string((uint8_t) game);
	nvs->set<HighScore, 5>(blob, score);
}

bool HighScoreManager::getHighScores(Games game, std::array<HighScore, 5>& score) const{
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return false;
	}

	const std::string blob = std::string("HighScore") + std::to_string((uint8_t) game);
	return nvs->get<HighScore, 5>(blob, score);
}