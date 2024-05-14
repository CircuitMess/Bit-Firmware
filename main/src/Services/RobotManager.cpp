#include "RobotManager.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"
#include "Screens/MainMenu/MainMenu.h"

const std::unordered_map<Games, RobotData> RobotManager::GameRobot = {
		{ Games::MrBee, { .robot = Robot::MrBee }},
		{ Games::Artemis, { .robot = Robot::Artemis }},
		{ Games::Bob, { .robot = Robot::Bob }},
		{ Games::Hertz, { .robot = Robot::Hertz }},
		{ Games::Robby, { .robot = Robot::Robby }},
		{ Games::Resistron, { .robot = Robot::Resistron }},
		{ Games::Capacitron, { .robot = Robot::Capacitron }},
		{ Games::Marv, { .robot = Robot::Marv }},
		{ Games::Buttons, { .robot = Robot::Buttons } },
		{ Games::Harald, { .token = Token::Harald } },
		{ Games::Frank, { .token = Token::Frank } },
		{ Games::RoboSpider, { .token = Token::RoboSpider } },
		{ Games::Fred, { .token = Token::Fred } },
		{ Games::Plank, { .token = Token::Plank } },
		{ Games::Dusty, { .token = Token::Dusty } },
		{ Games::Sparkly, { .token = Token::Sparkly } }
};

const std::unordered_map<Theme, RobotData> RobotManager::ThemeRobot = {
		{ Theme::Theme2, { .token = Token::CyberTheme } },
		{ Theme::Theme3, { .token = Token::FireTheme } },
		{ Theme::Theme4, { .token = Token::MountainTheme } }
};

const std::unordered_map<Pet, RobotData> RobotManager::PetRobot = {
		{ Pet::Cat, { .token = Token::CatPet } },
		{ Pet::Dog, { .token = Token::DogPet } },
		{ Pet::Parrot, { .token = Token::ParrotPet } },
		{ Pet::Cow, { .token = Token::CowPet } },
		{ Pet::Frog, { .token = Token::FrogPet } },
		{ Pet::Sheep, { .token = Token::SheepPet } }
};

RobotManager::RobotManager() : Threaded("GameMan", 2 * 1024, 5, 1), events(12){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	std::array<uint8_t, (uint8_t) Robot::COUNT + (uint8_t) Token::COUNT> robotsSeen = {0};
	if(nvs->get("Robots", robotsSeen)){
		for(int i = 0; i < (uint8_t) Robot::COUNT + (uint8_t) Token::COUNT; i++){
			if(robotsSeen[i]){
				unlocked[i] = true;
			}
		}
	}

	Events::listen(Facility::Robots, &events);
	start();
}

bool RobotManager::isUnlocked(Games game){
	if(!GameRobot.contains(game)) return true;
	auto rob = GameRobot.at(game);

	if(rob.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token];
	}

	return unlocked[(uint8_t) rob.robot];
}

bool RobotManager::isUnlocked(Theme theme){
	if(!ThemeRobot.contains(theme)) return true;
	auto rob = ThemeRobot.at(theme);

	if(rob.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token];
	}

	return unlocked[(uint8_t) rob.robot];
}

bool RobotManager::isUnlocked(Pet pet){
	if(!PetRobot.contains(pet)) return true;
	auto rob = PetRobot.at(pet);

	if(rob.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token];
	}

	return unlocked[(uint8_t) rob.robot];
}

void RobotManager::storeState(){
	const NVSFlash* nvs = (NVSFlash*) Services.get(Service::NVS);
	if(nvs == nullptr){
		return;
	}

	std::array<uint8_t, (uint8_t) Robot::COUNT + (uint8_t) Token::COUNT> robotsSeen = {0};
	nvs->get("Robots", robotsSeen);

	// Only unlocks, doesn't relock
	for(uint8_t i = 0; i < (uint8_t) Robot::COUNT + (uint8_t) Token::COUNT; ++i){
		robotsSeen[i] = unlocked[i] || robotsSeen[i];
	}

	nvs->set("Robots", robotsSeen);
}

void RobotManager::loop(){
	::Event evt;
	if(!events.get(evt, portMAX_DELAY)) return;

	auto data = (Robots::Event*) evt.data;
	RobotData rob = data->robot;

	if(data->action == Robots::Event::Insert){
		if(rob.robot >= Robot::COUNT && rob.token >= Token::COUNT){
			sendEvent(Event { .action = Event::Unknown }, Facility::Games);
			sendEvent(Event { .action = Event::Unknown }, Facility::Pets);
			sendEvent(Event { .action = Event::Unknown }, Facility::Themes);
			return;
		}

		bool isNew = false;
		const bool wasUnlocked = rob.robot >= Robot::COUNT ? unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token] : unlocked[(uint8_t) rob.robot];

		if(!wasUnlocked){
			isNew = true;
			unlocked[rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot] = true;
			storeState();
		}

		sendEvent(Event { .action = Event::Inserted, .rob = rob, .isNew = isNew }, Robots::isGame(rob) ? Facility::Games : Robots::isPet(rob) ? Facility::Pets : Robots::isTheme(rob) ? Facility::Themes : Facility::Games);
	}else if(data->action == Robots::Event::Remove){
		if(Robots::isGame(rob)){
			sendEvent(Event { .action = Event::Remove }, Facility::Games);
		}else if(Robots::isTheme(rob)){
			sendEvent(Event { .action = Event::Remove }, Facility::Themes);
		}else if(Robots::isPet(rob)){
			sendEvent(Event { .action = Event::Remove }, Facility::Pets);
		}
	}

	free(evt.data);
}

void RobotManager::sendEvent(RobotManager::Event evt, Facility type){
	MainMenu::gameEvent(evt);
	Events::post(type, evt);
}
