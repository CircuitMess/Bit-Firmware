#ifndef BIT_FIRMWARE_ROBOTS_H
#define BIT_FIRMWARE_ROBOTS_H

#include "Util/Threaded.h"
#include "Pins.hpp"

enum class Robot : uint8_t {
		MrBee,
		Resistron,
		Artemis,
		Robby,
		Marv,
		Capacitron,
		Bob,
		Buttons,
		Hertz,
		COUNT
};

enum class Token : uint8_t {
	Harald,
	Frank,
	Charlie,
	Fred,
	Planck,
	Dusty,
	Sparkly,
	CowPet,
	ParrotPet,
	FrogPet,
	DogPet,
	SheepPet,
	MountainTheme,
	CyberTheme,
	FireTheme,
	CatPet,
	COUNT
};

struct RobotData {
public:
	inline bool operator < (const RobotData& other) const {
		if((robot != Robot::COUNT || other.robot != Robot::COUNT) && robot != other.robot){
			return robot < other.robot;
		}

		return token < other.token;
	}

	Robot robot = Robot::COUNT;
	Token token = Token::COUNT;
};

class Robots : private SleepyThreaded {
public:
	Robots();
	~Robots() override;

	struct Event {
		enum { Insert, Remove } action;
		RobotData robot;
	};

	RobotData getInserted();

	bool testerBob();

	inline static constexpr bool isPet(RobotData robot){
		if(robot.robot != Robot::COUNT){
			return false;
		}

		constexpr const Token Pets[] = { Token::CatPet, Token::CowPet, Token::DogPet, Token::FrogPet, Token::ParrotPet, Token::SheepPet };
		return std::find(std::begin(Pets), std::end(Pets), robot.token) != std::end(Pets);
	}

	inline static constexpr bool isTheme(RobotData robot){
		if(robot.robot != Robot::COUNT){
			return false;
		}

		constexpr const Token Themes[] = { Token::CyberTheme, Token::FireTheme, Token::MountainTheme };
		return std::find(std::begin(Themes), std::end(Themes), robot.token) != std::end(Themes);
	}

	inline static constexpr bool isGame(RobotData robot){
		return !isTheme(robot) && !isPet(robot);
	}

private:
	static constexpr uint32_t CheckInterval = 500; // [ms]

	const int AddrPins[6] = { Pins::get(Pin::Addr1), Pins::get(Pin::Addr2), Pins::get(Pin::Addr3), Pins::get(Pin::Addr4), Pins::get(Pin::Addr5), Pins::get(Pin::Addr6) };
	const int CtrlPins[6] = { Pins::get(Pin::Ctrl1), Pins::get(Pin::Ctrl2), Pins::get(Pin::Ctrl3), Pins::get(Pin::Ctrl4), Pins::get(Pin::Ctrl5), Pins::get(Pin::Ctrl6) };

	bool inserted = false;
	RobotData current = { Robot::COUNT, Token::COUNT };

	void sleepyLoop() override;

	bool checkInserted();
	uint8_t checkAddr() const;
	uint8_t checkToken() const;

	static constexpr uint8_t TesterBobAddr = (uint8_t) Robot::Bob | 0b00100000;
};


#endif //BIT_FIRMWARE_ROBOTS_H
