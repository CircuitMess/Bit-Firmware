#include "Harald.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "Services/HighScoreManager.h"
#include "Util/Services.h"
#include "Util/stdafx.h"

Harald::Harald::Harald(Sprite& canvas) : Game(canvas, Games::Harald, "/Games/Harald", {
		{ "/bg.raw", {}, true },
		{ Icons[0], {}, true },
		{ Icons[1], {}, true },
		{ Icons[2], {}, true },
		{ Icons[3], {}, true },
		{ Icons[4], {}, true },
		{ Icons[5], {}, true },
		{ Icons[6], {}, true },
		{ Icons[7], {}, true },
		{ Icons[8], {}, true },
		{ Icons[9], {}, true },
		{ Icons[10], {}, true } }){

}

void Harald::Harald::onLoad(){
	Game::onLoad();

	auto bg = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile("/bg.raw"), PixelDim{ 128, 128 }),
			nullptr
	);
	bg->getRenderComponent()->setLayer(-1);
	addObject(bg);

	const size_t x1 = rand() % 4;
	const size_t y1 = rand() % 4;
	size_t x2 = rand() % 4;
	size_t y2 = rand() % 4;

	if(x1 == x2){
		x2 = (x2 + 1) % 4;
	}

	if(y1 == y2){
		y2 = (y2 + 1) % 4;
	}

	for(int x = 0; x < 4; ++x){
		for(int y = 0; y < 4; ++y){
			if((x == x1 && y == y1) || (x == x2 && y == y2)){
				continue;
			}

			elements[x][y].id = 0;
			elements[x][y].gameObj = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/Tile01.raw"), PixelDim{ 25, 25 }),
					nullptr
			);

			elements[x][y].gameObj->getRenderComponent()->setLayer(0);
			elements[x][y].gameObj->setPos(glm::vec2{14, 23} + glm::vec2{x * 25, y * 25});

			addObject(elements[x][y].gameObj);
		}
	}

	const uint8_t type1 = (rand() % 2) + 1;
	const uint8_t type2 = (rand() % 2) + 1;

	elements[x1][y1].id = type1;
	elements[x1][y1].gameObj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(Icons[type1]), PixelDim{ 25, 25 }),
			nullptr
	);

	elements[x1][y1].gameObj->getRenderComponent()->setLayer(0);
	elements[x1][y1].gameObj->setPos(glm::vec2{14, 23} + glm::vec2{x1 * 25, y1 * 25});

	addObject(elements[x1][y1].gameObj);

	elements[x2][y2].id = type2;
	elements[x2][y2].gameObj = std::make_shared<GameObject>(
			std::make_unique<StaticRC>(getFile(Icons[type2]), PixelDim{ 25, 25 }),
			nullptr
	);

	elements[x2][y2].gameObj->getRenderComponent()->setLayer(0);
	elements[x2][y2].gameObj->setPos(glm::vec2{14, 23} + glm::vec2{x2 * 25, y2 * 25});

	addObject(elements[x2][y2].gameObj);

	scoreElement = std::make_unique<Score>("POPULATION");
	scoreElement->setScore(score);
	addObject(scoreElement->getGO());
	scoreElement->getGO()->getRenderComponent()->setLayer(2);
	scoreElement->getGO()->setPos({ 2, 2 });

	uint32_t bestScore = 0;
	if(const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore)){
		bestScore = hsm->getAll(Games::Harald).front().score;
	}

	bestScoreElement = std::make_unique<Score>("BEST");
	bestScoreElement->setScore(bestScore);
	addObject(bestScoreElement->getGO());
	bestScoreElement->getGO()->getRenderComponent()->setLayer(2);
	bestScoreElement->getGO()->setPos({ 2, 12 });
}

void Harald::Harald::onLoop(float deltaTime){
	Game::onLoop(deltaTime);

	bool foundPair = false;

	for(int x = 0; x < 4; ++x){
		for(int y = 0; y < 4; ++y){
			if(elements[x][y].id == 0){
				foundPair = true;
				break;
			}

			if(x >= 0 && x < 3){
				if(elements[x + 1][y].id == elements[x][y].id){
					foundPair = true;
					break;
				}
			}

			if(x > 0){
				if(elements[x - 1][y].id == elements[x][y].id){
					foundPair = true;
					break;
				}
			}

			if(y >= 0 && y < 3){
				if(elements[x][y + 1].id == elements[x][y].id){
					foundPair = true;
					break;
				}
			}

			if(y > 0){
				if(elements[x][y - 1].id == elements[x][y].id){
					foundPair = true;
					break;
				}
			}
		}
	}

	if(!foundPair){
		//Lose condition
		audio.play({{ 500, 500, 400 },
					{ 0,   0,   100 },
					{ 300, 300, 400 },
					{ 500, 300, 150 },
					{ 0,   0,   100 },
					{ 400, 200, 150 },
					{ 100, 100, 400 },
				   });

		delayMillis(2000);

		exit();
		return;
	}
}

void Harald::Harald::handleInput(const Input::Data& data){
	Game::handleInput(data);

	if(data.action != Input::Data::Action::Press){
		return;
	}

	std::array<std::array<uint8_t, 4>, 4> idMap = {};

	for(int x = 0; x < 4; ++x){
		for(int y = 0; y < 4; ++y){
			idMap[x][y] = elements[x][y].id;
		}
	}

	if(data.btn == Input::Button::Up){
		for(int x = 0; x < 4; ++x){
			for(int target = 0; target < 4; ++target){
				if(idMap[x][target] == 0){
					for(int y = target + 1; y < 4; ++y){
						if(idMap[x][y] != 0){
							idMap[x][target] = idMap[x][y];
							idMap[x][y] = 0;
							--target;
							break;
						}
					}

					continue;
				}

				for(int y = target + 1; y < 4; ++y){
					if(idMap[x][y] == idMap[x][target]){
						idMap[x][y] = 0;
						score += std::pow(2, ++idMap[x][target]);
						break;
					}

					if(idMap[x][y] != 0){
						break;
					}
				}
			}
		}
	}else if(data.btn == Input::Button::Down){
		for(int x = 0; x < 4; ++x){
			for(int target = 3; target >= 0; --target){
				if(idMap[x][target] == 0){
					for(int y = target -1; y >= 0; --y){
						if(idMap[x][y] != 0){
							idMap[x][target] = idMap[x][y];
							idMap[x][y] = 0;
							++target;
							break;
						}
					}

					continue;
				}

				for(int y = target - 1; y >= 0; --y){
					if(idMap[x][y] == idMap[x][target]){
						idMap[x][y] = 0;
						score += std::pow(2, ++idMap[x][target]);
						break;
					}

					if(idMap[x][y] != 0){
						break;
					}
				}
			}
		}
	}else if(data.btn == Input::Button::Left){
		for(int y = 0; y < 4; ++y){
			for(int target = 0; target < 4; ++target){
				if(idMap[target][y] == 0){
					for(int x = target + 1; x < 4; ++x){
						if(idMap[x][y] != 0){
							idMap[target][y] = idMap[x][y];
							idMap[x][y] = 0;
							--target;
							break;
						}
					}

					continue;
				}

				for(int x = target + 1; x < 4; ++x){
					if(idMap[x][y] == idMap[target][y]){
						idMap[x][y] = 0;
						score += std::pow(2, ++idMap[target][y]);
						break;
					}

					if(idMap[x][y] != 0){
						break;
					}
				}
			}
		}
	}else if(data.btn == Input::Button::Right){
		for(int y = 0; y < 4; ++y){
			for(int target = 3; target >= 0; --target){
				if(idMap[target][y] == 0){
					for(int x = target -1; x >= 0; --x){
						if(idMap[x][y] != 0){
							idMap[target][y] = idMap[x][y];
							idMap[x][y] = 0;
							++target;
							break;
						}
					}

					continue;
				}

				for(int x = target - 1; x >= 0; --x){
					if(idMap[x][y] == idMap[target][y]){
						idMap[x][y] = 0;
						score += std::pow(2, ++idMap[target][y]);
						break;
					}

					if(idMap[x][y] != 0){
						break;
					}
				}
			}
		}
	}

	scoreElement->setScore(score);

	size_t numOfSame = 0;
	for(int x = 0; x < 4; ++x){
		for(int y = 0; y < 4; ++y){
			if(elements[x][y].id == idMap[x][y]){
				++numOfSame;
				continue;
			}

			elements[x][y].id = idMap[x][y];
			StaticRC* rc = (StaticRC*) elements[x][y].gameObj->getRenderComponent().get();
			rc->setFile(getFile(Icons[idMap[x][y]]));

			//Win condition
			if(idMap[x][y] >= 10){
				exit();
				return;
			}
		}
	}

	//No progress
	if(numOfSame == 4 * 4){
		audio.play({{ 100, 100, 100 },
					{ 0,   0,   75 },
					{ 100, 100, 100 },
				   });
		return;
	}

	audio.play({{ 300, 500, 150 },
				{ 0,   0,   50 },
				{ 100, 80, 75 },
			   });
	std::vector<uint8_t> validX;

	for(int x = 0; x < 4; ++x){
		for(int y = 0; y < 4; ++y){
			if(elements[x][y].id == 0){
				validX.emplace_back(x);
			}
		}
	}

	if(validX.empty()){
		return;
	}

	const uint8_t newX = validX[rand() % validX.size()];

	std::vector<uint8_t> validY;

	for(int y = 0; y < 4; ++y){
		if(elements[newX][y].id == 0){
			validY.emplace_back(y);
		}
	}

	if(validY.empty()){
		return;
	}

	const uint8_t newY = validY[rand() % validY.size()];

	elements[newX][newY].id = (1.0f * rand()) / INT_MAX > 0.65f ? 2 : 1;
	StaticRC* rc = (StaticRC*) elements[newX][newY].gameObj->getRenderComponent().get();
	rc->setFile(getFile(Icons[elements[newX][newY].id]));
}
