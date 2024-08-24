#include "RobotManager.h"
#include "Settings/Settings.h"
#include "Util/Services.h"
#include "Periph/NVSFlash.h"
#include "Screens/MainMenu/MainMenu.h"
#include "LV_Interface/FSLVGL.h"

const std::unordered_map<Games, RobotData> RobotManager::GameRobot = {
		{ Games::MrBee, { .robot = Robot::MrBee }},
		{ Games::Artemis,    { .robot = Robot::Artemis }},
		{ Games::Bob,        { .robot = Robot::Bob }},
		{ Games::Hertz,      { .robot = Robot::Hertz }},
		{ Games::Robby,      { .robot = Robot::Robby }},
		{ Games::Resistron,  { .robot = Robot::Resistron }},
		{ Games::Capacitron, { .robot = Robot::Capacitron }},
		{ Games::Marv,       { .robot = Robot::Marv }},
		{ Games::Buttons,    { .robot = Robot::Buttons } },
		{ Games::Harald,     { .token = Token::Harald } },
		{ Games::Frank,      { .token = Token::Frank } },
		{ Games::Charlie,    { .token = Token::Charlie } },
		{ Games::Fred,       { .token = Token::Fred } },
		{ Games::Planck,     { .token = Token::Planck } },
		{ Games::Dusty,      { .token = Token::Dusty } },
		{ Games::Sparkly,    { .token = Token::Sparkly } }
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

RobotManager::RobotManager() : Threaded("RobotManager", 5 * 1024, 5, 1), events(12){
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

bool RobotManager::isUnlocked(Games game) const{
	if(game == Games::COUNT){
		return false;
	}

	if(!GameRobot.contains(game)) return true;
	auto rob = GameRobot.at(game);

	if(rob.robot == Robot::COUNT && rob.token == Token::COUNT){
		return false;
	}

	if(rob.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token];
	}

	return unlocked[(uint8_t) rob.robot];
}

bool RobotManager::isUnlocked(Theme theme) const{
	if(theme == Theme::COUNT){
		return false;
	}

	if(!ThemeRobot.contains(theme)) return true;
	auto rob = ThemeRobot.at(theme);

	if(rob.robot == Robot::COUNT && rob.token == Token::COUNT){
		return false;
	}

	if(rob.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token];
	}

	return unlocked[(uint8_t) rob.robot];
}

bool RobotManager::isUnlocked(Pet pet) const{
	if(pet == Pet::COUNT){
		return false;
	}

	if(!PetRobot.contains(pet)) return true;
	auto rob = PetRobot.at(pet);

	if(rob.robot == Robot::COUNT && rob.token == Token::COUNT){
		return false;
	}

	if(rob.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) rob.token];
	}

	return unlocked[(uint8_t) rob.robot];
}

bool RobotManager::isUnlocked(RobotData robot) const{
	if(robot.robot == Robot::COUNT && robot.token == Token::COUNT){
		return false;
	}

	if(robot.robot >= Robot::COUNT){
		return unlocked[(uint8_t) Robot::COUNT + (uint8_t) robot.token];
	}

	return unlocked[(uint8_t) robot.robot];
}

Games RobotManager::toGame(RobotData robot) {
	for(auto entry : GameRobot){
		if(entry.second.token == robot.token && entry.second.robot == robot.robot){
			return entry.first;
		}
	}

	return Games::COUNT;
}

Theme RobotManager::toTheme(RobotData robot) {
	for(auto entry : ThemeRobot){
		if(entry.second.token == robot.token && entry.second.robot == robot.robot){
			return entry.first;
		}
	}

	return Theme::COUNT;
}

Pet RobotManager::toPet(RobotData robot) {
	for(auto entry : PetRobot){
		if(entry.second.token == robot.token && entry.second.robot == robot.robot){
			return entry.first;
		}
	}

	return Pet::COUNT;
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
		const bool wasUnlocked = unlocked[rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot];

		if(!wasUnlocked){
			isNew = true;
			unlocked[rob.robot >= Robot::COUNT ? (uint8_t) Robot::COUNT + (uint8_t) rob.token : (uint8_t) rob.robot] = true;
			storeState();
		}

		if(Robots::isGame(rob)){
			if(isNew){
				FSLVGL::menuChange();
			}

			sendEvent(Event { .action = Event::Inserted, .rob = rob, .isNew = isNew }, Facility::Games);
		}else if(Robots::isTheme(rob)){
			sendEvent(Event { .action = Event::Inserted, .rob = rob, .isNew = isNew }, Facility::Themes);
		}else if(Robots::isPet(rob)){
			sendEvent(Event { .action = Event::Inserted, .rob = rob, .isNew = isNew }, Facility::Pets);
		}
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
