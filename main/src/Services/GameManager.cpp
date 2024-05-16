#include "GameManager.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"
#include "Screens/MainMenu/MainMenu.h"

const std::unordered_map<Games, Robot> GameManager::GameRobot = {
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

GameManager::GameManager() : Threaded("GameMan", 2 * 1024, 5, 1), events(12){
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
	if(!GameRobot.contains(game)) return true;
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
		Robot rob = data->robot;
		if(rob >= Robot::COUNT){
			sendEvent(Event { .action = Event::Unknown });
			return;
		}

		bool isNew = false;
		if(!unlocked.count(rob)){
			isNew = true;
			unlocked.insert(rob);
			storeState();
		}

		sendEvent(Event { .action = Event::Inserted, .rob = rob, .isNew = isNew });
	}else if(data->action == Robots::Event::Remove){
		sendEvent(Event { .action = Event::Remove });
	}

	free(evt.data);
}

void GameManager::sendEvent(GameManager::Event evt){
	MainMenu::gameEvent(evt);
	Events::post(Facility::Games, evt);
}
