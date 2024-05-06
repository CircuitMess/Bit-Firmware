#include "HighScoreManager.h"
#include "Periph/NVSFlash.h"
#include "Util/Services.h"
#include <algorithm>

HighScoreManager::HighScoreManager(){
	for(size_t i = 0; i < (size_t) Games::COUNT; ++i){
		if(getHighScores((Games) i, highScores[i])){
			std::sort(highScores[i].begin(), highScores[i].end(), [](const HighScore& first, const HighScore& second) {
				return first.score > second.score;
			});
		}else{
			setHighScores((Games) i, highScores[i]);
		}
	}
}

bool HighScoreManager::isHighScore(Games game, uint32_t score) const{
	if(score == 0){
		return false;
	}

	for(const HighScore& highScore : highScores[(size_t) game]){
		if(highScore.score == score){
			return false;
		}
	}

	return highScores[(size_t) game].back().score != 0 || highScores[(size_t) game].back().score < score;
}

void HighScoreManager::saveScore(Games game, HighScore score){
	highScores[(size_t) game].back() = score;
	std::sort(highScores[(size_t) game].begin(), highScores[(size_t) game].end(), [](const HighScore& first, const HighScore& second) {
		return first.score > second.score;
	});
	setHighScores(game, highScores[(size_t) game]);
}

const std::array<HighScore, 5>& HighScoreManager::getAll(Games game) const{
	return highScores[(size_t) game];
}

bool HighScoreManager::hasHighScore(Games game) const{
	if(!hasScore(game)){
		return false;
	}

	return std::ranges::any_of(highScores[(size_t) game], [](HighScore score) { return score.score > 0; });
}

bool HighScoreManager::hasScore(Games game) const{
	static constexpr const std::array<Games, 8> NoScoreGames = { Games::Artemis,
																 Games::Robby,
																 Games::Bob,
																 Games::Buttons,
																 Games::Hertz,
																 Games::Marv,
																 Games::Pong,
																 Games::COUNT};

	return std::find(NoScoreGames.begin(), NoScoreGames.end(), game) == NoScoreGames.end();
}

void HighScoreManager::setHighScores(Games game, const std::array<HighScore, 5>& score) {
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	const std::string blob = std::string("HighScore") + std::to_string((uint8_t) game);
	nvs->set<HighScore, 5>(blob, score);
}

bool HighScoreManager::getHighScores(Games game, std::array<HighScore, 5>& score) {
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return false;
	}

	const std::string blob = std::string("HighScore") + std::to_string((uint8_t) game);
	return nvs->get<HighScore, 5>(blob, score);
}