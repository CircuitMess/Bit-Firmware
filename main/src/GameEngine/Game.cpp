#include "Game.h"
#include <utility>
#include <esp_log.h>

Game::Game(Sprite& base, const char* root, std::vector<ResDescriptor> resources) : collision(this), inputQueue(12), base(base), resMan(root),
																				   resources(std::move(resources)),
																				   loadTask([this](){ loadFunc(); }, "loadTask", 4096), render(this, base){

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

void Game::removeObject(const GameObjPtr& obj){
	collision.removeObject(*obj);
	objects.erase(obj);
}

void Game::handleInput(const Input::Data& data){

}

void Game::loop(uint micros){
	Event e;
	if(inputQueue.get(e, 0)){
		if(e.facility == Facility::Input){
			handleInput(*((Input::Data*) e.data));
		}
		free(e.data);
	}

	collision.update(micros);
	onLoop((float) micros / 1000000.0f);

	render.update(micros);
	onRender(base);

	// collision.drawDebug(CircuitPet.getDisplay()->getBaseSprite());
}

void Game::onStart(){}

void Game::onStop(){}

void Game::onLoad(){}

void Game::onLoop(float deltaTime){}

void Game::onRender(Sprite& canvas){}
