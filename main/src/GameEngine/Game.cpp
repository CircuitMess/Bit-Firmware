#include "Game.h"
#include <utility>
#include <esp_log.h>
#include "Util/Services.h"
#include "UIThread.h"
#include "Screens/Game/GameMenuScreen.h"
#include "Screens/MainMenu/MainMenu.h"
#include "Util/Notes.h"
#include "Services/HighScoreManager.h"
#include "Screens/Game/AwardsScreen.h"
#include "Services/AchievementSystem.h"

Game::Game(Sprite& base, Games gameType, const char* root, std::vector<ResDescriptor> resources) :
		collision(this), inputQueue(12), audio(*(ChirpSystem*) Services.get(Service::Audio)), gameType(gameType), base(base),
		resMan(root), resources(std::move(resources)),
		loadTask([this](){ loadFunc(); }, "loadTask", 4096, 12, 0),
		render(this, base){
	exited = false;
}

Game::~Game(){

}

void Game::load(){
	if(loaded || loadTask.running()) return;

	loadTask.start();
}

void Game::loadFunc(){
	resMan.load(resources);
	onLoad();
	loaded = true;
	loadTask.stop(0);
}

void Game::start(){
	if(started) return;

	if(!loaded){
		ESP_LOGE("Game", "Attempting to start game that wasn't loaded.");
		return;
	}

	started = true;
	onStart();
	Events::listen(Facility::Input, &inputQueue);
}

void Game::stop(){
	if(!started) return;
	started = false;
	onStop();
	Events::unlisten(&inputQueue);
}

bool Game::isLoaded() const{
	return loaded;
}

File Game::getFile(const std::string& path){
	return resMan.getResource(std::move(path));
}

void Game::addObject(const GameObjPtr& obj){
	objects.insert(obj);
}

void Game::addObjects(std::initializer_list<const GameObjPtr> objs){
	for(auto obj : objs){
		addObject(obj);
	}
}

void Game::removeObject(const GameObjPtr& obj){
	collision.removeObject(*obj);
	objects.erase(obj);
}

void Game::removeObjects(std::initializer_list<const GameObjPtr> objs){
	for(auto obj : objs){
		removeObject(obj);
	}
}

void Game::handleInput(const Input::Data& data){

}

void Game::exit(){
	exited = true;

	AchievementSystem* achievementSystem = (AchievementSystem*) Services.get(Service::Achievements);
	if(achievementSystem == nullptr){
		return;
	}

	std::vector<AchievementData> achievements;
	achievementSystem->endSession(achievements);

	auto ui = (UIThread*) Services.get(Service::UI);
	if(ui == nullptr){
		return;
	}

	const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	const uint32_t score = getScore();
	const uint32_t xp = getXP();
	const Games type = getType();

	if(hsm->isHighScore(type, score) || xp != 0/* || got new achievement*/){
		ui->startScreen([type, score, xp](){ return std::make_unique<AwardsScreen>(type, score, xp); });
		return;
	}

	ui->startScreen([type](){
		return std::make_unique<GameMenuScreen>(type);
	});
}

void Game::loop(uint micros){
	Event e;
	if(inputQueue.get(e, 0)){
		if(e.facility == Facility::Input){
			auto data = (Input::Data*) e.data;
			if(data->btn == Input::Menu && data->action == Input::Data::Release){
				stop();
					audio.play({ { NOTE_E6, NOTE_E6, 100 },
								  { NOTE_C6, NOTE_C6, 100 },
								  { NOTE_E6, NOTE_E6, 100 },
								  { NOTE_C6, NOTE_C6, 100 } });

				auto ui = (UIThread*) Services.get(Service::UI);
				ui->pauseGame();

				free(e.data);
				return;
			}

			handleInput(*data);
		}
		free(e.data);

		if(exited) return;
	}

	collision.update(micros);
	onLoop((float) micros / 1000000.0f);
	if(exited) return;

	render.update(micros);
	onRender(base);

//	collision.drawDebug(base);

	if(robot){
		robot->loop(micros);
	}
}

void Game::onStart(){
	AchievementSystem* achievementSystem = (AchievementSystem*) Services.get(Service::Achievements);
	if(achievementSystem == nullptr){
		return;
	}

	achievementSystem->startSession();
}

void Game::onStop(){
	AchievementSystem* achievementSystem = (AchievementSystem*) Services.get(Service::Achievements);
	if(achievementSystem == nullptr){
		return;
	}

	std::vector<AchievementData> achievements;
	achievementSystem->endSession(achievements);
}

void Game::onLoad(){}

void Game::onLoop(float deltaTime){}

void Game::onRender(Sprite& canvas){}

void Game::setRobot(std::shared_ptr<RoboCtrl::RobotDriver> robot){
	this->robot = robot;
}

