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
#include "Services/LEDService/LEDService.h"
#include "Services/TwinkleService.h"
#include "Devices/SinglePwmLED.h"

Game::Game(Sprite& base, Games gameType, const char* root, std::vector<ResDescriptor> resources) :
		collision(this), inputQueue(12), audio(*(ChirpSystem*) Services.get(Service::Audio)), gameType(gameType), base(base),
		resMan(root), resources(std::move(resources)),
		loadTask([this](){ loadFunc(); }, "loadTask", 4096, 12, 0),
		render(this, base){
	exited = false;

	buttons = GameButtonsUsed[(uint8_t) gameType];
	ledService = (LEDService*) Services.get(Service::LED);
	achievementSystem = (AchievementSystem*) Services.get(Service::Achievements);
}

Game::~Game(){

}

void Game::load(Allocator* alloc){
	if(loaded || loadTask.running()) return;

	this->alloc = alloc;
	loadTask.start();
}

void Game::loadFunc(){
	resMan.load(resources, alloc);
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

	auto input = (Input*) Services.get(Service::Input);
	for(auto& i: PwmMappings){
		ledService->remove(i.first);
	}
	uint8_t channel = 3;
	if(buttons.up){
		auto pwmInfo = PwmMappings.at(LED::Up);
		ledService->add<SinglePwmLED>(LED::Up, pwmInfo.pin, (ledc_channel_t) channel++, pwmInfo.limit);
		if(!input->isPressed(Input::Up)){
			ledService->on(LED::Up);
		}
	}
	if(buttons.down){
		auto pwmInfo = PwmMappings.at(LED::Down);
		ledService->add<SinglePwmLED>(LED::Down, pwmInfo.pin, (ledc_channel_t) channel++, pwmInfo.limit);
		if(!input->isPressed(Input::Down)){
			ledService->on(LED::Down);
		}
	}
	if(buttons.left){
		auto pwmInfo = PwmMappings.at(LED::Left);
		ledService->add<SinglePwmLED>(LED::Left, pwmInfo.pin, (ledc_channel_t) channel++, pwmInfo.limit);
		if(!input->isPressed(Input::Left)){
			ledService->on(LED::Left);
		}
	}
	if(buttons.right){
		auto pwmInfo = PwmMappings.at(LED::Right);
		ledService->add<SinglePwmLED>(LED::Right, pwmInfo.pin, (ledc_channel_t) channel++, pwmInfo.limit);
		if(!input->isPressed(Input::Right)){
			ledService->on(LED::Right);
		}
	}
	if(buttons.a){
		auto pwmInfo = PwmMappings.at(LED::A);
		ledService->add<SinglePwmLED>(LED::A, pwmInfo.pin, (ledc_channel_t) channel++, pwmInfo.limit);
		if(!input->isPressed(Input::A)){
			ledService->on(LED::A);
		}
	}
	if(buttons.b){
		auto pwmInfo = PwmMappings.at(LED::B);
		ledService->add<SinglePwmLED>(LED::B, pwmInfo.pin, (ledc_channel_t) channel++, pwmInfo.limit);
		if(!input->isPressed(Input::B)){
			ledService->on(LED::B);
		}
	}

	achievementSystem->startSession();

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

void Game::addAchi(Achievement id, int32_t increment){
	achievementSystem->increment(id, increment);
}

void Game::handleInput(const Input::Data& data){

}

void Game::exit(){
	exited = true;

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

	if(hsm->isHighScore(type, score) || xp != 0 || !achievements.empty()){
		if(auto twinkle = (TwinkleService*) Services.get(Service::Twinkle)){
			twinkle->start();
		}

		ui->startScreen([type, score, xp, &achievements](){ return std::make_unique<AwardsScreen>(type, score, xp, achievements); });
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
			if(data->btn == Input::Menu && data->action == Input::Data::Press){
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

			handleLEDs(*data);
			handleInput(*data);
		}
		free(e.data);

		if(exited) return;
	}

	collision.update(micros);
	onLoop((float) micros / 1000000.0f);
	if(exited) return;

	preRender(base);
	render.update(micros);
	onRender(base);

//	collision.drawDebug(base);

	if(robot){
		robot->loop(micros);
	}
}

void Game::onStart(){

}

void Game::onStop(){

}

void Game::onLoad(){}

void Game::onLoop(float deltaTime){}

void Game::preRender(Sprite& canvas){}

void Game::onRender(Sprite& canvas){}

void Game::setRobot(std::shared_ptr<RoboCtrl::RobotDriver> robot){
	this->robot = robot;
}

void Game::flashAll(){
	if(buttons.up) ledService->blink(LED::Up, FlashCount, FlashPeriod);
	if(buttons.down) ledService->blink(LED::Down, FlashCount, FlashPeriod);
	if(buttons.left) ledService->blink(LED::Left, FlashCount, FlashPeriod);
	if(buttons.right) ledService->blink(LED::Right, FlashCount, FlashPeriod);
	if(buttons.a) ledService->blink(LED::A, FlashCount, FlashPeriod);
	if(buttons.b) ledService->blink(LED::B, FlashCount, FlashPeriod);
}

void Game::handleLEDs(const Input::Data& data){
	if(data.btn == Input::Up && buttons.up){
		(data.action == Input::Data::Press) ? ledService->off(LED::Up, false) : ledService->on(LED::Up, false);
	}
	if(data.btn == Input::Down && buttons.down){
		(data.action == Input::Data::Press) ? ledService->off(LED::Down, false) : ledService->on(LED::Down, false);
	}
	if(data.btn == Input::Left && buttons.left){
		(data.action == Input::Data::Press) ? ledService->off(LED::Left, false) : ledService->on(LED::Left, false);
	}
	if(data.btn == Input::Right && buttons.right){
		(data.action == Input::Data::Press) ? ledService->off(LED::Right, false) : ledService->on(LED::Right, false);
	}
	if(data.btn == Input::A && buttons.a){
		(data.action == Input::Data::Press) ? ledService->off(LED::A, false) : ledService->on(LED::A, false);
	}
	if(data.btn == Input::B && buttons.b){
		(data.action == Input::Data::Press) ? ledService->off(LED::B, false) : ledService->on(LED::B, false);
	}
}
