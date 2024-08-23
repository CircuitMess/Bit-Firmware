#include "Harald.h"
#include "GameEngine/Rendering/StaticRC.h"
#include "GameEngine/Rendering/AnimRC.h"
#include "GameEngine/Collision//RectCC.h"
#include "Services/HighScoreManager.h"
#include "Util/Services.h"
#include "Util/stdafx.h"
#include <esp_random.h>

Harald::Harald::Harald(Sprite& canvas) : Game(canvas, Games::Harald, "/Games/Harald", {
		{ "/bg.raw", {}, true },
		{ "/Puf.gif", {}, true },
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
	bg->getRenderComponent()->setLayer(-2);
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

	for(int x = 0; x < 4; x++){
		for(int y = 0; y < 4; y++){
			auto bgObj = std::make_shared<GameObject>(
					std::make_unique<StaticRC>(getFile("/Tile01.raw"), PixelDim{ 25, 25 }),
					nullptr
			);
			bgObj->getRenderComponent()->setLayer(-1);
			bgObj->setPos(elements[x][y].gameObj->getPos());
			bgTiles.emplace_back(bgObj);
			addObject(bgObj);
		}
	}

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
	checkPufs(deltaTime);

	if(exiting){
		exitAnim(deltaTime);
		return;
	}

	tileMoveAnim(deltaTime);
}

void Harald::Harald::handleInput(const Input::Data& data){
	if(exiting) return;

	if(data.action != Input::Data::Action::Press){
		return;
	}

	if(data.btn == Input::A || data.btn == Input::B || data.btn == Input::Menu) return;

	if(!tileMoves.empty()) return;

	findMoves(data.btn);
}

void Harald::Harald::findMoves(Input::Button dir){
	IdField resultingIds = {};
	for(int x = 0; x < 4; ++x){
		for(int y = 0; y < 4; ++y){
			resultingIds[x][y] = elements[x][y].id;
		}
	}

	std::set<TileMove, TileMoveComp> foundMoves;
	uint32_t score = this->score;

	const auto addMove = [&foundMoves](const TileMove& move){
		if(foundMoves.contains(move)){
			foundMoves.erase(move);
		}
		foundMoves.insert(move);
	};

	if(dir == Input::Button::Up){
		for(int x = 0; x < 4; ++x){
			for(int target = 0; target < 4; ++target){
				if(resultingIds[x][target] == 0){
					for(int y = target + 1; y < 4; ++y){
						if(resultingIds[x][y] != 0){
							resultingIds[x][target] = resultingIds[x][y];
							resultingIds[x][y] = 0;

							addMove(TileMove {
									.source = { x, y },
									.target = { x, target },
									.combo = false
							});

							--target;
							break;
						}
					}

					continue;
				}

				for(int y = target + 1; y < 4; ++y){
					if(resultingIds[x][y] == resultingIds[x][target]){
						resultingIds[x][y] = 0;
						resultingIds[x][target]++;
						score += std::pow(2, resultingIds[x][target]);

						addMove(TileMove {
								.source = { x, y },
								.target = { x, target },
								.combo = true
						});

						break;
					}

					if(resultingIds[x][y] != 0){
						break;
					}
				}
			}
		}
	}else if(dir == Input::Button::Down){
		for(int x = 0; x < 4; ++x){
			for(int target = 3; target >= 0; --target){
				if(resultingIds[x][target] == 0){
					for(int y = target -1; y >= 0; --y){
						if(resultingIds[x][y] != 0){
							resultingIds[x][target] = resultingIds[x][y];
							resultingIds[x][y] = 0;

							addMove(TileMove {
									.source = { x, y },
									.target = { x, target },
									.combo = false
							});

							++target;
							break;
						}
					}

					continue;
				}

				for(int y = target - 1; y >= 0; --y){
					if(resultingIds[x][y] == resultingIds[x][target]){
						resultingIds[x][y] = 0;
						resultingIds[x][target]++;
						score += std::pow(2, resultingIds[x][target]);

						addMove(TileMove {
								.source = { x, y },
								.target = { x, target },
								.combo = true
						});

						break;
					}

					if(resultingIds[x][y] != 0){
						break;
					}
				}
			}
		}
	}else if(dir == Input::Button::Left){
		for(int y = 0; y < 4; ++y){
			for(int target = 0; target < 4; ++target){
				if(resultingIds[target][y] == 0){
					for(int x = target + 1; x < 4; ++x){
						if(resultingIds[x][y] != 0){
							resultingIds[target][y] = resultingIds[x][y];
							resultingIds[x][y] = 0;

							addMove(TileMove {
									.source = { x, y },
									.target = { target, y },
									.combo = false
							});

							--target;
							break;
						}
					}

					continue;
				}

				for(int x = target + 1; x < 4; ++x){
					if(resultingIds[x][y] == resultingIds[target][y]){
						resultingIds[x][y] = 0;
						resultingIds[target][y]++;
						score += std::pow(2, resultingIds[target][y]);

						addMove(TileMove {
								.source = { x, y },
								.target = { target, y },
								.combo = true
						});

						break;
					}

					if(resultingIds[x][y] != 0){
						break;
					}
				}
			}
		}
	}else if(dir == Input::Button::Right){
		for(int y = 0; y < 4; ++y){
			for(int target = 3; target >= 0; --target){
				if(resultingIds[target][y] == 0){
					for(int x = target -1; x >= 0; --x){
						if(resultingIds[x][y] != 0){
							resultingIds[target][y] = resultingIds[x][y];
							resultingIds[x][y] = 0;

							addMove(TileMove {
									.source = { x, y },
									.target = { target, y },
									.combo = false
							});

							++target;
							break;
						}
					}

					continue;
				}

				for(int x = target - 1; x >= 0; --x){
					if(resultingIds[x][y] == resultingIds[target][y]){
						resultingIds[x][y] = 0;
						resultingIds[target][y]++;
						score += std::pow(2, resultingIds[target][y]);

						addMove(TileMove {
								.source = { x, y },
								.target = { target, y },
								.combo = true
						});

						break;
					}

					if(resultingIds[x][y] != 0){
						break;
					}
				}
			}
		}
	}

	//No progress
	if(foundMoves.empty()){
		audio.play({{ 100, 100, 100 },
					{ 0,   0,   75 },
					{ 100, 100, 100 },
				   });
		return;
	}

	for(const auto& move : foundMoves){
		elements[move.source.x][move.source.y].gameObj->getRenderComponent()->setLayer(1);
	}

	tileMoves = foundMoves;
	moveResult = {
			.field = resultingIds,
			.score = score
	};

	audio.play({{ 300, 500, 150 }, { 0,   0,   50 }});
}

void Harald::Harald::applyMove(){
	score = moveResult.score;
	scoreElement->setScore(score);

	bool comboFound = false;

	for(const auto& move : tileMoves){
		const auto sourceId = moveResult.field[move.source.x][move.source.y];
		const auto targetId = moveResult.field[move.target.x][move.target.y];

		if(move.combo){
			comboFound = true;

			GameObjPtr pufObj = std::make_shared<GameObject>(std::make_unique<AnimRC>(getFile("/Puf.gif"), true), nullptr);
			pufObj->setPos(elements[move.target.x][move.target.y].gameObj->getPos() + glm::vec2 { -4, -7 });
			auto pufRc = (AnimRC*) pufObj->getRenderComponent().get();
			pufRc->setLayer(5);
			pufRc->setLoopMode(GIF::Single);
			pufRc->start();
			addObject(pufObj);

			pufs.emplace_back(Puf { pufObj, 0 });
		}

		// Change target tile
		elements[move.target.x][move.target.y].id = targetId;
		StaticRC* rc = (StaticRC*) elements[move.target.x][move.target.y].gameObj->getRenderComponent().get();
		rc->setFile(getFile(Icons[targetId]));

		// Change source tile
		elements[move.source.x][move.source.y].id = sourceId;
		elements[move.source.x][move.source.y].gameObj->setPos(glm::vec2{14, 23} + glm::vec2{move.source.x * 25, move.source.y * 25});
		rc = (StaticRC*) elements[move.source.x][move.source.y].gameObj->getRenderComponent().get();
		rc->setFile(getFile(Icons[sourceId]));
		rc->setLayer(0);

		//Win condition
		if(targetId >= 10){
			won = true;
		}
	}

	if(won){
		gameWin();
		return;
	}

	if(comboFound){
		audio.play({{ 100, 80, 75 }});
	}

	spawnNew();

	checkLoseCondition();
}

void Harald::Harald::spawnNew(){
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

void Harald::Harald::checkLoseCondition(){
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
		gameLose();
	}
}

void Harald::Harald::tileMoveAnim(float dt){
	if(tileMoves.empty()) return;

	tileMoveT = std::min(1.0f, tileMoveT + dt * TileMoveSpeed);

	float t = 1.0f - std::pow(1.0f - tileMoveT, 3.0f);

	for(const auto& move : tileMoves){
		GameObjPtr movingTile = elements[move.source.x][move.source.y].gameObj;
		const auto sourcePos = glm::vec2{14, 23} + glm::vec2{move.source.x * 25, move.source.y * 25};
		const auto targetPos = glm::vec2{14, 23} + glm::vec2{move.target.x * 25, move.target.y * 25};

		const auto gotoPos = glm::vec<2, float>{
				std::round((float) sourcePos.x + (float) (targetPos.x - sourcePos.x) * t),
				std::round((float) sourcePos.y + (float) (targetPos.y - sourcePos.y) * t)
		};

		movingTile->setPos(gotoPos);
	}

	if(tileMoveT >= 1.0f){
		applyMove();
		tileMoveT = 0;
		tileMoves.clear();
	}
}

void Harald::Harald::checkPufs(float dt){
	if(pufs.empty()) return;

	std::vector<Puf> forRemoval;

	for(auto& puf : pufs){
		puf.time += dt;
		if(puf.time >= PufDuration){
			forRemoval.push_back(puf);
		}
	}

	for(const auto& puf : forRemoval){
		removeObject(puf.go);
		std::erase_if(pufs, [puf](const Puf& other){ return puf.go == other.go; });
	}
}

void Harald::Harald::gameWin(){
	audio.play({{ 400, 600, 400 },
				{ 0,   0,   75 },
				{ 500, 700, 300 },
				{ 0,   0,   75 },
				{ 800, 800, 100 },
				{ 0,   0,   75 },
				{ 800, 800, 100 },
				{ 0,   0,   75 },
				{ 800, 800, 100 },
			   });

	exitStart();
}

void Harald::Harald::gameLose(){
	audio.play({{ 500, 500, 400 },
				{ 0,   0,   100 },
				{ 300, 300, 400 },
				{ 500, 300, 150 },
				{ 0,   0,   100 },
				{ 400, 200, 150 },
				{ 100, 100, 400 },
			   });

	exitStart();
}

void Harald::Harald::exitStart(){
	if(exiting) return;
	exiting = true;

	for(const auto& obj : bgTiles){
		removeObject(obj);
	}

	exitObjs.reserve(4*4);

	static constexpr int32_t ExitAnimYPush = 50;
	static constexpr uint32_t MaxSpeedX = 50;
	static constexpr uint32_t MinSpeedX = 20;

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			glm::vec2 spd = {
					(float) (esp_random() % (MaxSpeedX - MinSpeedX)) + MinSpeedX,
					won ? -(25.0f + 25.0f * ((float) esp_random() / (float) UINT32_MAX)) : (5.0f + 5.0f * ((float) esp_random() / (float) UINT32_MAX))
			};

			if(esp_random() % 2 == 0){
				spd.x *= -1;
			}

			ExitObj& obj = exitObjs.emplace_back(ExitObj { elements[i][j].gameObj, spd });

			obj.go->setCollisionComponent(std::make_shared<RectCC>(PixelDim{ 25, 25 }));
			collision.wallBot(*exitObjs.back().go, [&obj, this](){
				obj.speed.y *= won ? -1 : -0.5f;
			});

			if(elements[i][j].id != 0){
				obj.go->getRenderComponent()->setLayer(1);
			}
		}
	}
}

void Harald::Harald::exitAnim(float dt){
	if(!exiting) return;

	exitT += dt;
	if(exitT < (won ? ExitPauseWin : ExitPauseLose)) return;

	static constexpr float Gravity = 125.0f;

	bool visible = false;

	for(auto& obj : exitObjs){
		obj.speed.y += Gravity * dt;

		const auto newPos = obj.go->getPos() + obj.speed * dt;
		obj.go->setPos(newPos);

		visible |= !(newPos.x < -25 || newPos.x > 128 || newPos.y > 128);
	}

	if(!visible){
		exit();
	}
}
