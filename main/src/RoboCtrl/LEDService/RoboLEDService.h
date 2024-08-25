#ifndef PERSE_ROVER_LEDSERVICE_H
#define PERSE_ROVER_LEDSERVICE_H

#include <cstdint>
#include <map>
#include <tuple>
#include <driver/ledc.h>
#include <memory>
#include <mutex>
#include "Util/Threaded.h"
#include "Util/Queue.h"
#include "Pins.hpp"


enum class LED : uint8_t {
	Up, Down, Left, Right, A, B, Menu, RobotCtrl1, RobotCtrl2, RobotCtrl3, RobotCtrl4, RobotCtrl5, RobotCtrl6, COUNT
};
struct PwmMappingInfo {
	gpio_num_t pin = GPIO_NUM_NC;
	uint8_t limit = 100;
};
const std::map<LED, PwmMappingInfo> PwmMappings = {
		{ LED::Up,    { (gpio_num_t) LED_UP,    0x10 }},
		{ LED::Down,  { (gpio_num_t) LED_DOWN,  0x10 }},
		{ LED::Left,  { (gpio_num_t) LED_LEFT,  0x10 }},
		{ LED::Right, { (gpio_num_t) LED_RIGHT, 0x10 }},
		{ LED::A,     { (gpio_num_t) LED_A,     0x10 }},
		{ LED::B,     { (gpio_num_t) LED_B,     0x10 }},
		{ LED::Menu,  { (gpio_num_t) LED_MENU,  0x10 }}
};

class RoboLEDService : private Threaded {
public:
	explicit RoboLEDService();

	virtual ~RoboLEDService();

	template<typename T, typename... Args>
	void add(LED led, Args&& ... args){
		T* dev = new T(std::forward<Args>(args)...);

		ledDevices[led] = dev;
	}

	void remove(LED led);

	void on(LED led, bool interrupt = true);

	void off(LED led, bool interrupt = true);

	void blink(LED led, uint32_t count = 1, uint32_t period = 1000);

	void set(LED led, float percent, bool interrupt = true);

	inline bool isRegistered(LED led) const{
		return ledDevices.contains(led);
	}

protected:
	virtual void loop() override;

private:
	enum LEDInstruction {
		On,
		Off,
		Blink,
		Set
	};

	struct LEDInstructionInfo {
		LED led;
		LEDInstruction instruction;
		uint32_t count;
		uint32_t period;
		float targetPercent;
		bool interrupt;
	};

	std::map<LED, class SingleLED*> ledDevices;
	std::map<LED, std::unique_ptr<class LEDFunction>> ledFunctions;
	Queue<LEDInstructionInfo> instructionQueue;

private:
	void onInternal(LED led);

	void offInternal(LED led);

	void blinkInternal(LED led, uint32_t count, uint32_t period);

	void setInternal(LED led, float percent);
};

#endif //PERSE_ROVER_LEDSERVICE_H