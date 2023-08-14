#include "GameManager.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

static const std::unordered_map<Games, Robot> GameRobot = {
		{ Games::MrBee, Robot::MrBee },
		{ Games::Artemis, Robot::Artemis },
		{ Games::Bob, Robot::Bob },
		{ Games::Hertz, Robot::Hertz },
		{ Games::Robby, Robot::Robby },
		{ Games::Resistron, Robot::Resistron },
		{ Games::Capacitron, Robot::Capacitron },
		{ Games::Marv, Robot::Marv },
		{ Games::Buttons, Robot::Buttons }
};

GameManager::GameManager() : Threaded("GameMan", 2 * 1024), events(12){
	auto set = (Settings*) Services.get(Service::Settings);
	const auto all = set->get();
	for(int i = 0; i < Robot::COUNT; i++){
		if(all.robotsSeen[i]){
			unlocked.insert((Robot) i);
		}
	}

	Events::listen(Facility::Robots, &events);
	start();
}

bool GameManager::isUnlocked(Games game){
	if(!GameRobot.contains(game)) return false;
	auto rob = GameRobot.at(game);
	return unlocked.count(rob);
}

void GameManager::storeState(){
	auto set = (Settings*) Services.get(Service::Settings);
	auto all = set->get();

	// Only unlocks, doesn't relock
	for(const Robot rob : unlocked){
		all.robotsSeen[rob] = 1;
	}

	set->set(all);
	set->store();
}

void GameManager::loop(){
	::Event evt;
	if(!events.get(evt, portMAX_DELAY)) return;

	auto data = (Robots::Event*) evt.data;
	if(data->action == Robots::Event::Insert){
		bool isNew = false;
		Robot rob = data->robot;
		if(!unlocked.count(rob)){
			isNew = true;
			unlocked.insert(rob);
			storeState();
		}

		Events::post(Facility::Games, Event { .action = Event::Inserted, .rob = rob, .isNew = isNew });
	}

	free(evt.data);
}
